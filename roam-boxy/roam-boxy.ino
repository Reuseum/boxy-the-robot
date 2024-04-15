/*         Boxy~the~robot 
    created by :kyle
    For: Reuseum Educational 
        this is boxy the roamer
        controll code
        uses ultrasonic sensor
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
WebSocketsServer webSocket = WebSocketsServer(81);

Servo servo;

const int ledPin = 16;
const int motor1A = 5;
const int motor1B = 4;
const int motor2A = 14;
const int motor2B = 12;
const int maxSpeed = 255;
const int trigPin = 6; // Define your ultrasonic sensor pins
const int echoPin = 3;

bool roamingStarted = false;

void handleWebSocketMessage(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String message = String((char*)payload);
    Serial.print("Received WebSocket message: ");
    Serial.println(message);
    if (message == "led_on") {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED State: OFF");
    } else if (message == "led_off") {
      digitalWrite(ledPin, LOW);
      Serial.println("LED State: ON");
    } else if (message.startsWith("servo")) {
      int angle = message.substring(6).toInt();
      servo.write(angle);
      Serial.print("Servo Angle: ");
      Serial.println(angle);
    } else if (message.startsWith("speed")) {
      int speed = message.substring(6).toInt();
      // Adjust motor speed based on the received value
      Serial.print("Speed: ");
      Serial.println(speed);
    } else if (message == "start_roaming") {
      Serial.println("Start Roaming");
      roamingStarted = true; // Set the flag to true when roaming is started
    } else if (message == "stop_roaming") {
      Serial.println("Stop Roaming");
      roamingStarted = false; // Set the flag to false when roaming is stopped
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Setup access point
  WiFi.softAP(apSSID, apPassword);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Initialize pins
  pinMode(ledPin, OUTPUT);
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);

  servo.attach(14);
  servo.write(90); // Set initial servo angle to 90 degrees

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  // Serve "roam.html" from SPIFFS when root URL is requested
  server.serveStatic("/", SPIFFS, "/roam.html");

  // Start the server
  server.begin();
  Serial.println("HTTP server started");

  webSocket.begin();
  webSocket.onEvent(handleWebSocketMessage);
}

void loop() {
  webSocket.loop();
  server.handleClient(); // Handle incoming client requests

  // Check if roaming has started before executing roaming behavior
  if (roamingStarted) {
    // Read distance from ultrasonic sensor
    long duration, distance;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;

    // If obstacle detected within a certain range, avoid it
    if (distance < 20) {
      randomTurn();
    } else {
      moveForward();
    }
  }
}

void moveForward() {
  analogWrite(motor1A, maxSpeed);
  digitalWrite(motor1B, LOW);
  analogWrite(motor2A, maxSpeed);
  digitalWrite(motor2B, LOW);
  Serial.println("Forward");
}

void randomTurn() {
  // Randomly choose left or right turn
  if (random(2) == 0) {
    // Turn left
    analogWrite(motor1A, 0);
    digitalWrite(motor1B, maxSpeed);
    analogWrite(motor2A, maxSpeed);
    digitalWrite(motor2B, LOW);
    Serial.println("Left");
  } else {
    // Turn right
    analogWrite(motor1A, maxSpeed);
    digitalWrite(motor1B, LOW);
    analogWrite(motor2A, 0);
    digitalWrite(motor2B, maxSpeed);
    Serial.println("Right");
  }
  delay(1000); // Wait for turn to complete
  moveForward(); // Move forward after random turn
}
