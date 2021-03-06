#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include <WebSocketsClient.h>
#include <Hash.h>

//https://github.com/esp8266/arduino-esp8266fs-plugin/releases java thingy

//https://www.hackster.io/brzi/nodemcu-websockets-tutorial-3a2013 documentation for sending data to front end

ESP8266WiFiMulti wifiMulti;       // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
// ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server(80);       // Create a webserver object that listens for HTTP request on port 80
WebSocketsServer webSocket(81);    // create a websocket server on port 81 for GPIO services

File fsUploadFile;                 // a File variable to temporarily store the received file

const char *ssid = "ESP8266 Access Point"; // The name of the Wi-Fi network that will be created
const char *password = "thereisnospoon";   // The password required to connect to it, leave blank for an open network

const char *OTAName = "ESP8266";           // A name and a password for the OTA service
const char *OTAPassword = "esp8266";


#define TRIG D1 
#define ECHO D2
#define GPIO_0 D0 //solenoid
#define GPIO_1 D5 //misc

#define GPIO_TEST D3

#define USE_SERIAL Serial1

long waterLevel0 = 0; //water level0
long waterLevel1 = 0; //water level1

long tankHeight0 = 50; //tank0 height
long tankHeight1 = 52; //tank1 height

void sonic_sensor0_setup(){
  Serial.begin(115200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  delay(500);
  
}

// void sonic_sensor1_setup(){
//   Serial.begin(115200);
//   pinMode(TRIG, OUTPUT);
//   pinMode(ECHO, INPUT);

//   Serial.print(hp);
//   Serial.print("\n");
//   delay(1000);
  
// }

int sonic_sensor0_loop(){
      
// Sending to computer
//  Serial.print("Current water level is: ");
  long t = 0, h = 0, hp = 0;

  
  // Transmitting pulse
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  // Waiting for pulse
  t = pulseIn(ECHO, HIGH);
  
  // Calculating distance 
  h = t / 58;
 
  h = h - 6;  // offset correction
  h = tankHeight0 - h;  // water height, 0 - 50 cm
  
  hp = 2 * h;  // distance in %, 0-100 %
  waterLevel0 = hp;
  return waterLevel0;
  
  delay(500);
}

// int sonic_sensor1_loop(){
      
// // Sending to computer
// //  Serial.print("Current water level is: ");
//   long t = 0, h = 0;

  
//   // Transmitting pulse
//   digitalWrite(TRIG, LOW);
//   delayMicroseconds(2);
//   digitalWrite(TRIG, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(TRIG, LOW);
  
//   // Waiting for pulse
//   t = pulseIn(ECHO, HIGH);
  
//   // Calculating distance 
//   h = t / 58;
 
//   h = h - 6;  // offset correction
//   h = tankHeight1 - h;  // water height, 0 - 50 cm
  
//   waterLevel1 = 2 * h;  // distance in %, 0-100 %
//   return waterLevel1;
//   // Serial.print(" cm\n");
//   // Serial.print("\n");
  
//   delay(1000);
// }

const char* mdnsName = "esp8266"; // Domain name for the mDNS responder

void setup() {
  sonic_sensor0_setup();

  Serial.begin(115200);        // Start the Serial communication to send messages to the computer

  startWiFi();                 // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  //check!
  
  startOTA();                  // Start the OTA service
  
  startSPIFFS();               // Start the SPIFFS and list all contents

  startWebSocket();            // Start a WebSocket server
  
  startMDNS();                 // Start the mDNS responder

  startServer();               // Start a HTTP server with a file read handler and an upload handler
  
}


unsigned long prevMillis = millis();

void loop() {
  sonic_sensor0_loop();
  webSocket.loop();                           // constantly check for websocket events
  server.handleClient();                      // run the server
  ArduinoOTA.handle();                        // listen for OTA events

  retrieveWaterLevel();

}



void startWiFi() { // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  WiFi.softAP(ssid, password);             // Start the access point
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started\r\n");

  wifiMulti.addAP("ssid_from_AP_1", "your_password_for_AP_1");   // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
  wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  Serial.println("Connecting");
  while (wifiMulti.run() != WL_CONNECTED && WiFi.softAPgetStationNum() < 1) {  // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }
  Serial.println("\r\n");
  if(WiFi.softAPgetStationNum() == 0) {      // If the ESP is connected to an AP
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());             // Tell us what network we're connected to
    Serial.print("IP address:\t");
    Serial.print(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer
  } else {                                   // If a station is connected to the ESP SoftAP
    Serial.print("Station connected to ESP8266 AP");
  }
  Serial.println("\r\n");
}

void startOTA() { // Start the OTA service
  ArduinoOTA.setHostname(OTAName);
  ArduinoOTA.setPassword(OTAPassword);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");

    //TO-DO: turn off all GPIO and sensors! ex. digitalWrite(ourpins, LOW);

  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\r\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready\r\n");
}

void startSPIFFS() { // Start the SPIFFS and list all contents
  SPIFFS.begin();                             // Start the SPI Flash File System (SPIFFS)
  Serial.println("SPIFFS started. Contents:");
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("\tFS File: %s, size: %s\r\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
  }
}

void startWebSocket() { // Start a WebSocket server
  webSocket.begin();                          // start the websocket server
  webSocket.onEvent(webSocketEvent);          // if there's an incomming websocket message, go to function 'webSocketEvent'
  Serial.println("WebSocket server started.");
}

void startMDNS() { // Start the mDNS responder
  MDNS.begin(mdnsName);                        // start the multicast domain name server
  Serial.print("mDNS responder started: http://");
  Serial.print(mdnsName);
  Serial.println(".local");
}

void startServer() { // Start a HTTP server with a file read handler and an upload handler
  server.on("/edit.html",  HTTP_POST, []() {  // If a POST request is sent to the /edit.html address,
    server.send(200, "text/plain", ""); 
  }, handleFileUpload);                       // go to 'handleFileUpload'

  server.onNotFound(handleNotFound);          // if someone requests any other file or page, go to function 'handleNotFound'
                                              // and check if the file exists

  server.begin();                             // start the HTTP server
  Serial.println("HTTP server started.");
}

void handleNotFound(){ // if the requested file or page doesn't exist, return a 404 not found error
  if(!handleFileRead(server.uri())){          // check if the file exists in the flash memory (SPIFFS), if so, send it
    server.send(404, "text/plain", "404: File Not Found");
  }
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

void handleFileUpload(){ // upload a new file to the SPIFFS
  HTTPUpload& upload = server.upload();
  String path;
  if(upload.status == UPLOAD_FILE_START){
    path = upload.filename;
    if(!path.startsWith("/")) path = "/"+path;
    if(!path.endsWith(".gz")) {                          // The file server always prefers a compressed version of a file 
      String pathWithGz = path+".gz";                    // So if an uploaded file is not compressed, the existing compressed
      if(SPIFFS.exists(pathWithGz))                      // version of that file must be deleted (if it exists)
         SPIFFS.remove(pathWithGz);
    }
    Serial.print("handleFileUpload Name: "); Serial.println(path);
    fsUploadFile = SPIFFS.open(path, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
    path = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile) {                                    // If the file was successfully created
      fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
      server.sendHeader("Location","/success.html");      // Redirect the client to the success page
      server.send(303);
    } else {
      server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}



String commands [] = {"setTankHeight", "toggleGPIO", "getWaterLevel"}; //a b c
// string arguments for commands[0:2] respectively: (toggleHIGH | toggleLOW), 
int gpioList [] = {0,1};
String tankList [] = {"tank0","tank1"};

int tmp = 0;
String tmpStr = "";
String tmpCommand = "";


void runGPIO(int pin, String IO){

  if (IO == "toggleHIGH")
  {
    if (pin == gpioList[0]) 
    {
      Serial.begin(115200);
          pinMode(GPIO_0, OUTPUT);
          digitalWrite(GPIO_0,HIGH);
    
    }

    if (pin == gpioList[1]) 
    {
      Serial.begin(115200);
          pinMode(GPIO_1, OUTPUT);
          digitalWrite(GPIO_1,HIGH);
    
    }

  }

  if (IO == "toggleLOW")
  {
    if (pin == gpioList[0]) 
    {
      Serial.begin(115200);
          pinMode(GPIO_0, OUTPUT);
          digitalWrite(GPIO_0,LOW);
    
    }

    if (pin == gpioList[1]) 
    {
      Serial.begin(115200);
          pinMode(GPIO_1, OUTPUT);
          digitalWrite(GPIO_1,LOW);
    
    }

  }
}

void setTank(int tankHeight, String whichTank){

  if (whichTank == "tank0")
  {
    tankHeight0 = tankHeight;
  }

  if (whichTank == "tank1")
  {
    tankHeight1 = tankHeight;
  }
}


    void magicFunction(int intData, String strData, String command){

    if (command == commands[0]){
    
      setTank(intData,strData);
    }

    if (command == commands[1])
    {
      runGPIO(intData, strData);
    }

    if (command == commands[2])
    {
      retrieveWaterLevel();
    }
    
}

String hackyString = retrieveWaterLevel();

    String retrieveWaterLevel(){

    char buffer[16];
    return itoa(waterLevel0,buffer,10);
  
    }

//https://stackoverflow.com/questions/14769917/when-to-use-unsigned-char-pointer
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) { // When a WebSocket message is received
  switch (type) {
    case WStype_DISCONNECTED:             // if the websocket is disconnected
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED: {              // if a new websocket connection is established
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
      
    case WStype_BIN:
      
      Serial.printf("[%u] get binary: %s\n", num, payload);
      hexdump(payload,length);

      if (payload[0] == 'a'){
        tmpCommand = commands[0];
        magicFunction(tmp,tmpStr,tmpCommand);
        break;
      }
      
      if (payload[0] == 'b'){
        tmpCommand = commands[1];
        magicFunction(tmp,tmpStr,tmpCommand);
        break;
      }
      
      if (payload[0] == 'c'){
        tmpCommand = commands[2];
        webSocket.broadcastTXT(hackyString);
        
        break;
      }

     break;
     
     case WStype_TEXT:                     // if new text data is received
    
      Serial.printf("[%u] get Text: %s\n", num, payload);

      if (isDigit(payload[0])){

        
        Serial.print("Integer detected.\n ");

        unsigned char* magicChunk = payload;

        const char* magicChar_asterisk(reinterpret_cast<const char*>(magicChunk)); //https://stackoverflow.com/questions/17746688/convert-unsigned-char-to-string

        int targetNumber = atoi(magicChar_asterisk);
        tmp = targetNumber;
        Serial.print(tmp);
        
      }

      else {

        Serial.print("String detected.\n ");

        unsigned char* magicChunk = payload;

        //const char* magicChar_asterisk(reinterpret_cast<const char*>(magicChunk)); //https://stackoverflow.com/questions/17746688/convert-unsigned-char-to-string

        String magicString(reinterpret_cast<char*>(magicChunk)); https://stackoverflow.com/questions/17746688/convert-unsigned-char-to-string
        
        //Serial.print(magicChar_asterisk);

        tmpStr = magicString;
        Serial.print(tmpStr);

            }
             
        break;
  }
}


String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
}

String getContentType(String filename) { // determine the filetype of a given filename, based on the extension
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}
