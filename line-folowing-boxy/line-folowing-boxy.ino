/*         Boxy~the~robot 
    created by :kyle
    For: Reuseum Educational 
        this is boxy line following
        controll code
        uses ir sensors
     .__________________.          
     |                  |
     |     ~~      ~~   |
-----|      #       #   |-----
{<><}|          ^       |{<><}
{<><}|__________________|{<><}
-----                    -----*/ 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Servo.h>
#include "FS.h"

const char* apSSID = "boxy";
const char* apPassword = "";

ESP8266WebServer server(80);
WebSocketsServer webSocket(81);
Servo servo;
const int servoPin = A0; // Define servo pin
const int ledPin = 16; // Define LED pin
const int motor1A = 5;
const int motor1B = 4;
const int motor2A = 12;
const int motor2B = 14;
const int leftSensorPin = 13; // Define left sensor pin
const int rightSensorPin = 15; // Define right sensor pin

int maxspeed = 255;
bool lineFollowing = false;

void handleWebSocketMessage(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String message = String((char*)payload);
    Serial.print("Received message: ");
    Serial.println(message);
    if (message == "led_on") {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED turned ON");
    } else if (message == "led_off") {
      digitalWrite(ledPin, LOW);
      Serial.println("LED turned OFF");
    } else if (message.startsWith("servo")) {
      int angle = message.substring(6).toInt();
      servo.write(angle);
      Serial.print("Set servo angle to: ");
      Serial.println(angle);
    } else if (message.startsWith("speed")) {
      int speed = message.substring(6).toInt();
      maxspeed = speed;
      Serial.print("Set max speed to: ");
      Serial.println(maxspeed);
    } else if (message == "start_following") {
      lineFollowing = true;
      Serial.println("Started following line");
    } else if (message == "stop_following") {
      lineFollowing = false;
      Serial.println("Stopped following line");
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup...");
  WiFi.softAP(apSSID, apPassword);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  pinMode(ledPin, OUTPUT);
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);
  pinMode(leftSensorPin, INPUT);
  pinMode(rightSensorPin, INPUT);
  servo.attach(servoPin);
  servo.write(90);
  
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }
  
  server.serveStatic("/", SPIFFS, "/line.html");
  server.begin();
  webSocket.begin();
  webSocket.onEvent(handleWebSocketMessage);
  
  Serial.println("Setup completed!");
}

void loop() {
  webSocket.loop();
  server.handleClient();
  
  if (lineFollowing) {
    lineFollow();
  }
}

void lineFollow() {
  int leftSensorValue = digitalRead(leftSensorPin);
  int rightSensorValue = digitalRead(rightSensorPin);

  Serial.print("Left sensor value: ");
  Serial.print(leftSensorValue);
  Serial.print(", Right sensor value: ");
  Serial.println(rightSensorValue);

  if (leftSensorValue == LOW && rightSensorValue == LOW) {
    analogWrite(motor1A, maxspeed);
    digitalWrite(motor1B, LOW);
    analogWrite(motor2A, maxspeed);
    digitalWrite(motor2B, LOW);
    Serial.print("forward");
  } else if (leftSensorValue == LOW && rightSensorValue == HIGH) {
    analogWrite(motor1A, maxspeed);
    digitalWrite(motor1B, LOW);
    analogWrite(motor2A, LOW);
    digitalWrite(motor2B, maxspeed);
    Serial.print("LEFT");
  } else if (leftSensorValue == HIGH && rightSensorValue == LOW) {
    analogWrite(motor1A, LOW);
    digitalWrite(motor1B, maxspeed);
    analogWrite(motor2A, maxspeed);
    digitalWrite(motor2B, LOW);
       Serial.print("RIGHT");
  } else {
    digitalWrite(motor1A, LOW);
    digitalWrite(motor1B, LOW);
    digitalWrite(motor2A, LOW);
    digitalWrite(motor2B, LOW);
       Serial.print("STOP");
  }
}
