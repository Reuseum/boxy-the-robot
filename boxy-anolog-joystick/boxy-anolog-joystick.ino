/*         Boxy~the~robot 
    created by :kyle
    For: Reuseum Educational 
        anolog joystick controll
        for boxy the robot
     .__________________.          
     |                  |
     |     ~~      ~~   |
-----|      #       #   |-----
{<><}|          ^       |{<><}
{<><}|__________________|{<><}
-----                    -----*/        

#include <ESP8266WiFi.h>
#include <Wire.h>

const char* ssid = "boxyAP";
const char* password = "YourWiFiPassword";
const char* serverAddress = "192.168.4.1"; // IP address of the server ESP8266
const int serverPort = 80; // Port on which the server is listening

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Initialize I2C communication
  Wire.begin();
}

void loop() {
  // Read analog value from potentiometer connected to PCF8591T channel 3
  int potValue = readAnalogInput();

  // Map potentiometer value (0-255) to pwmSlider value (100-1000)
  int pwmSliderValue = map(potValue, 0, 255, 100, 1000);

  // Read analog values from joystick
  int xValue = readAnalogX();
  int yValue = readAnalogY();

  // Send joystick and pwmSlider values to server
  sendValuesToServer(xValue, yValue, pwmSliderValue);

  delay(100); // Adjust delay as needed
}

int readAnalogInput() {
  // Read analog value from PCF8591T channel 3
  Wire.beginTransmission(0x48); // PCF8591T address
  Wire.write(0x03); // Control byte for channel 3
  Wire.endTransmission();

  Wire.requestFrom(0x48, 1); // Request 1 byte from PCF8591T
  return Wire.read(); // Return analog value (0-255)
}

int readAnalogX() {
  // Read X-axis analog value from PCF8591T channel 0
  Wire.beginTransmission(0x48); // PCF8591T address
  Wire.write(0x00); // Control byte for channel 0
  Wire.endTransmission();

  Wire.requestFrom(0x48, 1); // Request 1 byte from PCF8591T
  return Wire.read(); // Return analog value (0-255)
}

int readAnalogY() {
  // Read Y-axis analog value from PCF8591T channel 1
  Wire.beginTransmission(0x48); // PCF8591T address
  Wire.write(0x01); // Control byte for channel 1
  Wire.endTransmission();

  Wire.requestFrom(0x48, 1); // Request 1 byte from PCF8591T
  return Wire.read(); // Return analog value (0-255)
}

void sendValuesToServer(int xValue, int yValue, int pwmSliderValue) {
  // Establish TCP connection with the server
  WiFiClient client;
  if (!client.connect(serverAddress, serverPort)) {
    Serial.println("Connection failed");
    return;
  }

  // Construct the HTTP request with joystick and pwmSlider values
  String httpRequest = "GET /joystick?x=" + String(xValue) + "&y=" + String(yValue) +
                       "&pwm=" + String(pwmSliderValue) + " HTTP/1.1\r\n" +
                       "Host: " + String(serverAddress) + "\r\n" +
                       "Connection: keep-alive\r\n\r\n";

  // Send the HTTP request to the server
  client.print(httpRequest);

  // Wait for the server to respond
  while (client.connected()) {
    if (client.available()) {
      String response = client.readStringUntil('\r');
      Serial.println("Server response: " + response);
    }
  }
}

