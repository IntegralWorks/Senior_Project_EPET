/* Water Level Meter

Measuring water level with ultrasonic sensor HR S04.
HR S04 OPERATING VOLTAGE: 3.8-5.1 V

ESP8266 PINOUT: https://raw.githubusercontent.com/nodemcu/nodemcu-devkit-v1.0/master/Documents/NODEMCU_DEVKIT_V1.0_PINMAP.png
GPIO GUIDE: https://www.instructables.com/id/HOW-TO-use-the-ARDUINO-SERIAL-MONITOR/
NETWORKING: https://circuitdigest.com/microcontroller-projects/sending-arduino-data-to-webpage

Arduino IDE 1.5.8
*/

int trig = D0;
int echo = D1;
int gpio_0 = D2;
int gpio_1 = D7;

int ByteReceived;

void setup() /* remember in arduino this runs once.*/
{
  sonic_sensor_setup();
  gpio_setup();
}

void sonic_sensor_setup(){
  Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void gpio_setup(){
  Serial.begin(9600);
  pinMode(gpio_0, OUTPUT);
  pinMode(gpio_1, OUTPUT);

  Serial.println("---Start Serial Monitor SEND_RCVE ---");
    Serial.println(" Type in Box Above, .");
  Serial.println("(Decimal)(Hex)(Character)");
  Serial.println();
}

void sonic_sensor_loop(){
    long t = 0, h = 0, hp = 0;

  
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
  
  // Sending to computer
  Serial.print(hp);
  // Serial.print(" cm\n");
  Serial.print("\n");
  
  delay(1000);
}

void gpio_loop()   
{
  if (Serial.available() > 0)
  {
    ByteReceived = Serial.read();
    Serial.print(ByteReceived);   
    Serial.print("        ");      
    Serial.print(ByteReceived, HEX);
    Serial.print("       ");     
    Serial.print(char(ByteReceived));
    
    if(ByteReceived == 'A') // Single Quote! This is a character.
    {
      digitalWrite(gpio_0,HIGH);
      Serial.print(" gpio_0 ON ");
    }
    
    if(ByteReceived == 'B')
    {
      digitalWrite(gpio_0,LOW);
      Serial.print(" gpio_0 OFF");
    }

     if(ByteReceived == 'C') // Single Quote! This is a character.
    {
      digitalWrite(gpio_1,HIGH);
      Serial.print(" gpio_1 ON ");
    }
    
    if(ByteReceived == 'D')
    {
      digitalWrite(gpio_1,LOW);
      Serial.print(" gpio_1 OFF");
    }
    
    Serial.println();    // End the line

  }
}

void loop() /*remember in arduino this runs constantly.*/
{
  sonic_sensor_loop();
  gpio_loop();
}
