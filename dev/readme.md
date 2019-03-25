# readme
# Steps for installation

1. Install NodeMCU on Arduino IDE following this guide: https://dzone.com/articles/programming-the-esp8266-with-the-arduino-ide-in-3

2. Install this plugin found here: https://github.com/esp8266/arduino-esp8266fs-plugin/releases/download/0.3.0/ESP8266FS-0.3.0.zip
  (I don't exactly remember how to install it sorry.) 
  
3.  When your development enviroment is set up and you've compiled, uploaded your code to ESP8266, press Ctrl+K on the Arduino IDE and
click on the sketch. There will be a "data" folder. Compress index.html into an index.html.gz file (you'll need a program called 
7-Zip for that, just google 7-zip and install) by clicking left > 7-Zip > compress into archive. Then put index.html.gz into the data
folder. 

4. From Arduino IDE go to Tools > ESP8266 Sketch Data Upload. If this option does not appear, you must do step 2 until it works. (if 
at any time you have trouble with installing anything just tell me and I can help.) 

5. I know this is cumbersome (due to a bug right now), but bear with me: you'll need to connect to the ESP8266's WiFi access point on your 
computer with the password "thereisnospoon". This means you will lose Internet access if you are not A. connected to ethernet as well
or B. follow this guide (https://www.windowscentral.com/how-set-and-manage-network-bridge-connection-windows-10) [Note: I have
not actually tried that guide, but I will soon and it seems very simple to do.]
Then, you'll need to type in the IP address (because of a bug described below, you'll have to go to the "Properties" of the connection and 
get the IP address.)

6. Unfortunately there is a bug preventing mDNS from working, so you cannot type in http://esp8266.local. (If someone could provide a
bugfix that would be fantastic!)

7. Once you fufill step 5, type in the IP address of the ESP8266. Voila! You'll be taken to index.html, and you'll be able to test your 
code. 

Current warnings/suggestions:
File upload is kind of slow. To counter this, *always compile your backend code in arduino IDE and your frontend code in either
WebStorm IDE or a browser's console first!* 
Try to get ethernet access. It's really convenient to connect to the Internet with ethernet and to the ESP with Wifi, so you don't have to 
bridge to wifi connections.

BUGS:
mDNS is broken. Can we get a like, subscribe, and bug fix?
