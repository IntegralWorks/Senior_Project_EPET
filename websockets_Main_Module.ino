#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>
 
// Replace with your network credentials
const char* ssid = "Not_your_library's_wifi";
const char* password = "K33Per$0utW1ngDr0m3s";
 
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
 
String page = "";
int trig = D0;
int echo = D1;
int gpio_0 = D2;
int gpio_1 = D7;

long hp = 0; //sorry for the messy global :( 

void sonic_sensor_setup(){
  Serial.begin(115200);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  server.on("/WATER_LEVEL:", [](){
    server.send(200, "text/html", page);
    Serial.print(hp);
    Serial.print("\n");

    delay(1000);
 });
}

int sonic_sensor_loop(){
    
  
  // Sending to computer
//  Serial.print("Current water level is: ");
  long t = 0, h = 0;
  //hp = 0;

  
  // Transmitting pulse
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  // Waiting for pulse
  t = pulseIn(echo, HIGH);
  
  // Calculating distance 
  h = t / 58;
 
  h = h - 6;  // offset correction
  h = 50 - h;  // water height, 0 - 50 cm
  
  hp = 2 * h;  // distance in %, 0-100 %
  return hp;
  // Serial.print(" cm\n");
  // Serial.print("\n");
  
  delay(1000);
}
 
void setup(void){
  sonic_sensor_setup();
 //the HTML of the web page
 page = "<h1>Simple NodeMCU Web Server</h1><p><a href=\"GPIO_0_ON\"><button>ON</button></a>&nbsp;<a href=\"GPIO_0_OFF\"><button>OFF</button></a></p><h2>Sample text?</h2><p><a href=\"GPIO_1_ON\"><button>ON</button></a>&nbsp;<a href=\"GPIO_1_OFF\"><button>OFF</button></a></p><h3>Water level detection</h3><p><a href=\"WATER_LEVEL:\"><button>SEND TO SERIAL MONITOR</button></a>&nbsp;</p>";
 
 //make the LED pin output and initially turned off
 pinMode(gpio_0, OUTPUT);
 digitalWrite(gpio_0, LOW);
 pinMode(gpio_1, OUTPUT);
 digitalWrite(gpio_1, LOW);
 
 
 delay(1000);
 
 Serial.begin(115200);
 WiFi.begin(ssid, password); //begin WiFi connection
 Serial.println("");
 
 // Wait for connection
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
 }
 
 Serial.println("");
 Serial.print("Connected to ");
 Serial.println(ssid);
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());
 
 server.on("/", [](){
    server.send(200, "text/html", page);
 });
 
 server.on("/GPIO_0_ON", [](){
    server.send(200, "text/html", page);
    digitalWrite(gpio_0, HIGH);
    delay(1000);
 });
 
 server.on("/GPIO_0_OFF", [](){
    server.send(200, "text/html", page);
    digitalWrite(gpio_0, LOW);
    delay(1000);
 });

 server.on("/GPIO_1_ON", [](){
    server.send(200, "text/html", page);
    digitalWrite(gpio_1, HIGH);
    delay(1000);
 });
 
 server.on("/GPIO_1_OFF", [](){
    server.send(200, "text/html", page);
    digitalWrite(gpio_1, LOW);
    delay(1000);
 });
 
 server.begin();
 webSocket.begin();
 webSocket.onEvent(webSocketEvent);
 
 Serial.println("Web server started!");
}
 
void loop(void){
  sonic_sensor_loop();
  webSocket.loop();
  server.handleClient();
  if (Serial.available() > 0){
    char c[] = {(char)Serial.read()};
    webSocket.broadcastTXT(c, sizeof(c));
  }
}
 
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  if (type == WStype_TEXT){
   for(int i = 0; i < length; i++) Serial.print((char) payload[i]);
   Serial.println();
  }
}