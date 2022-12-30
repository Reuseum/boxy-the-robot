#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>       
#include <ESP8266HTTPUpdateServer.h>
#include "FS.h"
const char* ssid = "boxcar3";
const char* password = "";
//Set Wifi ssid and password;
uint8_t pin_led = 16
;
//Define the pins
//Motor B
int PWMB = 5; //Speed control-d1
int BIN1 = 4; //Direction-d2
//Motor A
int PWMA = 14; //Speed control -D5
int AIN1 = 12; //Direction -D6

// Declare a global variable to store the maximum motor speed
int maxspeed = 0;

ESP8266WebServer server (80);

//This function takes the parameters passed in the URL(the x and y coordinates of the joystick)
//and sets the motor speed based on those parameters. 
  boolean yDir;
void toggleLED()
{
  digitalWrite(pin_led,!digitalRead(pin_led));
}

void getLEDState()
{
  toggleLED();
  String led_state = digitalRead(pin_led) ? "ON" : "OFF";
  server.send(200,"text/plain", led_state);
}
void getslider(){
 //gets value from slider
  String value = server.arg("value");
// Convert the value to an integer
 maxspeed = value.toInt();
 Serial.print("maxspeed=");
 Serial.println(maxspeed);
  
}
void handleJSData(){

  int x = server.arg(0).toInt() * 10;
  int y = server.arg(1).toInt() * 10;
  
  Serial.print("x=");
  Serial.println(x);
  Serial.print("y=");
  Serial.println(y);
   

 
  int aSpeed = abs(y);
  int bSpeed = abs(y);
  //set the direction based on y being negative or positive
 

    

  //adjust to speed of each each motor depending on the x-axis
  //it slows down one motor and speeds up the other proportionately 
  //based on the amount of turning
  aSpeed = map(aSpeed + x/2,0,1000, 0, maxspeed);
  bSpeed = map(bSpeed - x/2,0,1000, 0, maxspeed);
aSpeed = constrain(aSpeed + x/2, 0, maxspeed);
bSpeed = constrain(bSpeed - x/2, 0, maxspeed);
  // Update the maximum motor speed with the value of value_int

  
//aSpeed/=2.0;
//bSpeed /=2.0;
  //use the speed and direction values to turn the motors
  //if either motor is going in reverse from what is expected,
  //just change the 2 digitalWrite lines for both motors:
  //!ydir would become ydir, and ydir would become !ydir
  //digitalWrite(STBY, HIGH);  
  Serial.print("aSpeed=");
  Serial.println(aSpeed);
  Serial.print("bSpeed=");
  Serial.println(bSpeed);
  Serial.print("maxspeed=");
  Serial.println(maxspeed);
  
   if ( y < 0 ){
    yDir = 0; 
      //MotorA
  digitalWrite(AIN1, yDir);
  analogWrite(PWMA, aSpeed);
  //MotorB
  digitalWrite(BIN1, yDir);
  analogWrite(PWMB, bSpeed);
  }
  else if (y >0)
  { 
    yDir = 0;
      //MotorA
  analogWrite(AIN1, aSpeed);
  digitalWrite(PWMA, yDir);
  //MotorB
  analogWrite(BIN1, bSpeed);
  digitalWrite(PWMB, yDir);

  }  
  else if (y == 0)
  {
  analogWrite(AIN1, LOW);
  digitalWrite(PWMA, LOW);
  //MotorB
  analogWrite(BIN1, LOW);
  digitalWrite(PWMB, LOW);

  }

  //return an HTTP 200
  server.send(200, "text/plain", "");   
}


void setup()
{
  pinMode(pin_led, OUTPUT);
  Serial.begin(115200);
  delay(100);
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  delay(100);


  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //set the pins as outputs

  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  
  
  //initialize SPIFFS to be able to serve up the static HTML files. 
  if (!SPIFFS.begin()){
    Serial.println("SPIFFS Mount failed");
  } 
  else {
    Serial.println("SPIFFS Mount succesfull");
  }
  //set the static pages on SPIFFS for the html and js
  server.serveStatic("/", SPIFFS, "/joystick.html"); 
  server.serveStatic("/virtualjoystick.js", SPIFFS, "/virtualjoystick.js");
  //call handleJSData function when this URL is accessed by the js in the html file
  server.on("/jsData.html", handleJSData);
  server.on("/ledstate",getLEDState);
  server.on("/slider",getslider);
  server.begin();
}

void loop()
{
  server.handleClient();  
}
