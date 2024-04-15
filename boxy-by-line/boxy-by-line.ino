/* Boxy~the~robot 
   created by: kyle
   For: Reuseum Educational 
   This is boxy controlled
   by line input code
   .__________________.          
   |                  |
   |     ~~      ~~   |
---|      #       #   |---
{<><}|          ^       |{<><}
{<><}|__________________|{<><}
---                    ---
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <Servo.h>
#include <WebSocketsServer.h>

const char* ssid = "boxy";
const char* password = "";

// Define motor pins
const int motor1a = 4;  // Replace with your actual pin numbers
const int motor1b = 5;
const int motor2a = 12;  // Replace with your actual pin numbers
const int motor2b = 14;

// Define LED pin
const int ledPin = 16;  // Replace with your actual pin number

// Define servo pin
const int servoPin = 3;  // Replace with your actual pin number

// Define servo object
Servo servo;
// Define motor speed variable
int pwm = 255; // Default PWM value (adjust as needed)

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

IPAddress ip(192, 168, 1, 1);  // Set your desired IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

void setup() {
    Serial.begin(115200);

    // Set up motor pins
    pinMode(motor1a, OUTPUT);
    pinMode(motor1b, OUTPUT);
    pinMode(motor2a, OUTPUT);
    pinMode(motor2b, OUTPUT);

    // Set up LED pin
    pinMode(ledPin, OUTPUT);

    // Set up servo
    servo.attach(servoPin);

    // Connect to Wi-Fi
    WiFi.softAPConfig(ip, gateway, subnet);
    WiFi.softAP(ssid, password);
    IPAddress apIP = WiFi.softAPIP();
    Serial.printf("Access Point IP address: %s\n", apIP.toString().c_str());

    // Initialize SPIFFS
    if (!SPIFFS.begin()) {
        Serial.println("Failed to mount file system");
        return;
    }

    // Serve "byline.html" from SPIFFS when root URL is requested
    server.serveStatic("/", SPIFFS, "/byline.html");

    // Start the server
    server.begin();
    Serial.println("HTTP server started");

    // Initialize WebSocket
    webSocket.begin();
    webSocket.onEvent(handleWebSocketMessage); // Set the WebSocket event handler
}

void loop() {
    server.handleClient();
    webSocket.loop();
}

// Function to handle WebSocket messages
void handleWebSocketMessage(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    String message = (char *)payload;
    Serial.printf("Received WebSocket message: %s\n", message.c_str());
    
    // Parse the message and execute corresponding actions
    if (message.startsWith("f,")) {
        // Move forward
        int time_ms_forward = message.substring(2).toInt();
        moveForward(time_ms_forward);
    } else if (message.startsWith("b,")) {
        // Move backward
        int time_ms_backward = message.substring(2).toInt();
        moveBackward(time_ms_backward);
    } else if (message.startsWith("r,")) {
        // Turn right
        int time_ms_right = message.substring(2).toInt();
        turnRight(time_ms_right);
    } else if (message.startsWith("l,")) {
        // Turn left
        int time_ms_left = message.substring(2).toInt();
        turnLeft(time_ms_left);
    } else if (message.startsWith("p,")) {
        // Set PWM
        pwm = message.substring(2).toInt();
        // Implement PWM setting (to be added)
        Serial.print("Setting PWM to: ");
        Serial.println(pwm);
    } else if (message.startsWith("s,")) {
        // Set servo position
        int servoPos = message.substring(2).toInt();
        servo.write(servoPos);
        Serial.print("Setting servo position to: ");
        Serial.println(servoPos);
    }

    // Introduce a delay before processing the next WebSocket message
    delay(500); // Adjust the delay as needed
}

void moveForward(int duration) {
    digitalWrite(motor1a, HIGH);
    digitalWrite(motor1b, LOW);
    digitalWrite(motor2a, HIGH);
    digitalWrite(motor2b, LOW);
    delay(duration);
    stopMotors();
    Serial.println("Moving forward");
}

void moveBackward(int duration) {
    digitalWrite(motor1a, LOW);
    digitalWrite(motor1b, HIGH);
    digitalWrite(motor2a, LOW);
    digitalWrite(motor2b, HIGH);
    delay(duration);
    stopMotors();
    Serial.println("Moving backward");
}

void turnRight(int duration) {
    digitalWrite(motor1a, HIGH);
    digitalWrite(motor1b, LOW);
    digitalWrite(motor2a, LOW);
    digitalWrite(motor2b, HIGH);
    delay(duration);
    stopMotors();
    Serial.println("Turning right");
}

void turnLeft(int duration) {
    digitalWrite(motor1a, LOW);
    digitalWrite(motor1b, HIGH);
    digitalWrite(motor2a, HIGH);
    digitalWrite(motor2b, LOW);
    delay(duration);
    stopMotors();
    Serial.println("Turning left");
}

void stopMotors() {
    digitalWrite(motor1a, LOW);
    digitalWrite(motor1b, LOW);
    digitalWrite(motor2a, LOW);
    digitalWrite(motor2b, LOW);
}
