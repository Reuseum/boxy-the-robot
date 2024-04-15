/*         Boxy~the~robot 
    created by :kyle
    For: Reuseum Educational 
        this is boxy dual-joystic
        controll code
     .__________________.          
     |                  |
     |     ~~      ~~   |
-----|      #       #   |-----
{<><}|          ^       |{<><}
{<><}|__________________|{<><}
-----                    -----*/ 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

// Set the access point credentials
const char* ssid = "boxy";   //wifi AP name
const char* password = "";  // password for AP

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Motor pins
const int motor1A = 5; // Motor 1 pin A
const int motor1B = 4; // Motor 1 pin B
const int motor2A = 14; // Motor 2 pin A
const int motor2B = 12; // Motor 2 pin B

// LED pin
const int ledPin = 16; // LED pin

// Variables to store motor speed
int motor1Speed = 0;
int motor2Speed = 0;

// LED state
bool ledState = false; // Initialize LED state

// Function to handle WebSocket events
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    if (strcmp((char*)payload, "toggleLED") == 0) {
      // Toggle LED state
      ledState = !ledState;

      // Send updated LED state back to the client who triggered the toggle
      String ledStateMessage = ledState ? "LEDState: OFF" : "LEDState: ON";
      webSocket.sendTXT(num, ledStateMessage);
    }
      
      // Handle joystick data
      StaticJsonDocument<200> doc;
      deserializeJson(doc, payload);
      int joy1Y = doc["joy1Y"];
      int joy2Y = doc["joy2Y"];
      
        Serial.println("joy1Y:");
        Serial.print(joy1Y);
        
        Serial.println("joy2Y:");
        Serial.print(joy2Y);
  
      // Map joystick values to appropriate PWM range for motor control
      motor1Speed = map(joy1Y, -100, 100, -255, 255);
      motor2Speed = map(joy2Y, -100, 100, -255, 255);
      
         Serial.println("motor1Speed:");
        Serial.print(motor1Speed);
        
        Serial.println("motor2Speed:");
        Serial.print(motor2Speed);

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

  // Connect to Wi-Fi network
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println("Wi-Fi AP created");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Route for root / web page
  server.serveStatic("/", SPIFFS, "/dual-stick.html");
  server.serveStatic("/VirtualJoystick.js", SPIFFS, "/VirtualJoystick.js");

  // Start HTTP server
  server.begin();

  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  server.handleClient();

  // Toggle LED state if requested
  digitalWrite(ledPin, ledState ? HIGH : LOW);
}
