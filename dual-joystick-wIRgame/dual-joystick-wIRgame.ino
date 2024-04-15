/*         Boxy~the~robot 
    created by :kyle
    For: Reuseum Educational 
        this is boxy the 
        dual-joystic controll
        with ir tag game code
     .__________________.          
     |                  |
     |     ~~      ~~   |
-----|      #       #   |-----
{<><}|          ^       |{<><}
{<><}|__________________|{<><}
-----                    -----*/ 
#define DECODE_SAMSUNG
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include <IRremote.h>
#include <ArduinoJson.h>

// Set the access point credentials
const char* ssid = "boxy";
const char* password = "";

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Motor pins
const int motor1A = 4; // Motor 1 pin A
const int motor1B = 5; // Motor 1 pin B
const int motor2A = 12; // Motor 2 pin A
const int motor2B = 14; // Motor 2 pin B

// Infrared sender and receiver   
int RECV_PIN = 13;   // Infrared module receive pin
int IR_SEND_PIN  = 15; // Infrared send pin
IRrecv irrecv(RECV_PIN); 
decode_results results; 
IRsend irsend(IR_SEND_PIN); // Create IRsend instance

// LED pin
const int ledPin = 16; // LED pin

// Variables to store motor speed
int motor1Speed = 0;
int motor2Speed = 0;

// LED state
bool ledState = false; // Initialize LED state

// Variable to track game state
bool gameRunning = false;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    if (strcmp((char*)payload, "toggleLED") == 0) {
      // Toggle LED state
      ledState = !ledState;
      digitalWrite(ledPin, ledState ? HIGH : LOW);

      // Send updated LED state back to the client who triggered the toggle
      String ledStateMessage = ledState ? "LEDState: OFF" : "LEDState: ON";
      webSocket.sendTXT(num, ledStateMessage);
      Serial.println(ledStateMessage);
      Serial.println("LED state toggled");
    } else if (strcmp((char*)payload, "startgame") == 0) {
      // Toggle game state
      gameRunning = !gameRunning;

      // Print message based on game state
      if (gameRunning) {
        Serial.println("Game started!");
      } else {
        Serial.println("Game stopped!");
      }
      Serial.println("Game state toggled");
    } else if (strcmp((char*)payload, "sendtag") == 0) {
      irsend.sendSAMSUNG(0xE0E040BF, 32); // Send tag signal
      Serial.println("Tag signal sent!");
      Serial.println("Tag signal sent");
    } else {
      // Handle JSON messages
      StaticJsonDocument<200> doc;
      deserializeJson(doc, payload);
      int joy1Y = doc["joy1Y"];
      int joy2Y = doc["joy2Y"];

      Serial.print("Received joy1Y: ");
      Serial.println(joy1Y);
      Serial.print("Received joy2Y: ");
      Serial.println(joy2Y);

      // Map joystick values to appropriate PWM range for motor control
      motor1Speed = map(joy1Y, -100, 100, -255, 255);
      motor2Speed = map(joy2Y, -100, 100, -255, 255);

      Serial.print("Calculated motor1Speed: ");
      Serial.println(motor1Speed);
      Serial.print("Calculated motor2Speed: ");
      Serial.println(motor2Speed);

      // Control motor 1 direction based on joystick values
      if (joy1Y < 0) { // Reverse
        analogWrite(motor1A, abs(motor1Speed));
        analogWrite(motor1B, 0);
      } else { // Forward
        analogWrite(motor1A, 0);
        analogWrite(motor1B, abs(motor1Speed));
      }

      // Control motor 2 direction based on joystick values
      if (joy2Y < 0) { // Reverse
        analogWrite(motor2A, abs(motor2Speed));
        analogWrite(motor2B, 0);
      } else { // Forward
        analogWrite(motor2A, 0);
        analogWrite(motor2B, abs(motor2Speed));
      }
    }
  }
}

void setup() {
  // Initialize Serial port
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }

  // Setup motor pins
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);

  // Setup LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Initially turn off LED

  // Infrared receiver setup
  irrecv.enableIRIn(); // Start the receiver
  irsend.begin(IR_SEND_PIN); 

  // Connect to Wi-Fi network
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println("Wi-Fi AP created");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Route for root / web page
  server.serveStatic("/", SPIFFS, "/dual-stickWgame.html");
  server.serveStatic("/VirtualJoystick.js", SPIFFS, "/VirtualJoystick.js");

  // Start HTTP server
  server.begin();

  // Register WebSocket event handler
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  server.handleClient();
}
