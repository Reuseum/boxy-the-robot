/*         Boxy~the~robot 
    created by :kyle
    For: Reuseum Educational 
        this is boxy single
        joystic websockets
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
#include <WebSocketsServer.h>
#include <FS.h>
#include <ArduinoJson.h>

// Access point credentials
const char* ssid = "boxy";
const char* password = "";

ESP8266WebServer server(80);
WebSocketsServer webSocket(81); // WebSocket server on port 81

// Motor pins
const int motor1A = 4; // Motor 1 pin A
const int motor1B = 5; // Motor 1 pin B
const int motor2A = 12; // Motor 2 pin A
const int motor2B = 14; // Motor 2 pin B

// LED pin
const int ledPin = 16; // LED pin

// Direction variable
bool yDir = false;

// Maximum speed for motors
int maxSpeed = 200; // Default value, can be changed via WebSocket

// WebSocket event handling function
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    if (type == WStype_TEXT) {
        String message((char*)payload);
        Serial.println("WebSocket message received: " + message);

        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, message);

        if (error) {
            Serial.println(String("Failed to parse WebSocket message: ") + error.c_str());
            return;
        }

        // Update maxSpeed if provided in the WebSocket message
        if (doc.containsKey("maxSpeed")) {
            maxSpeed = doc["maxSpeed"].as<int>();
        }

        if (doc.containsKey("joy1X") && doc.containsKey("joy1Y")) {
            int x = doc["joy1X"].as<int>() * 10;
            int y = doc["joy1Y"].as<int>() * 10;

            int aSpeed = abs(y);
            int bSpeed = abs(y);

            aSpeed = constrain(aSpeed + x / 2, 0, maxSpeed);
            bSpeed = constrain(bSpeed - x / 2, 0, maxSpeed);

            Serial.print("aSpeed=");
            Serial.println(aSpeed);
            Serial.print("bSpeed=");
            Serial.println(bSpeed);

            if (y < 0) {
                yDir = false; // Reverse direction
                // Motor 2
                digitalWrite(motor2B, yDir);
                analogWrite(motor2A, aSpeed);
                // Motor 1
                digitalWrite(motor1B, yDir);
                analogWrite(motor1A, bSpeed);
            } else if (y > 0) {
                yDir = true; // Forward direction
                // Motor 2
                analogWrite(motor2B, aSpeed);
                digitalWrite(motor2A, yDir);
                // Motor 1
                analogWrite(motor1B, bSpeed);
                digitalWrite(motor1A, yDir);
            } else {
                // Stop the motors if y is 0
                analogWrite(motor2B, LOW);
                digitalWrite(motor2A, LOW);
                analogWrite(motor1B, LOW);
                digitalWrite(motor1A, LOW);
            }
        }
    }
}

// Setup function for configuring the ESP8266
void setup() {
    pinMode(ledPin, OUTPUT);
    Serial.begin(115200);

    // Configuring the access point
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    // Mounting SPIFFS for serving static HTML and JS
    if (!SPIFFS.begin()) {
        Serial.println("SPIFFS mount failed");
    } else {
        Serial.println("SPIFFS mount successful");
    }

    // Serving static files from SPIFFS
    server.serveStatic("/", SPIFFS, "/singlejoy.html");
    server.serveStatic("/VirtualJoystick.js", SPIFFS, "/VirtualJoystick.js");

    // Starting the WebSocket server and setting the event handler
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
     server.begin();
}

// Main loop to handle WebSocket and HTTP server events
void loop() {
    server.handleClient();  
    webSocket.loop(); // Handle WebSocket events
}
