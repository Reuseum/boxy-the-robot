![DSC02828](https://user-images.githubusercontent.com/121644329/210115160-8a283f81-5afd-439f-b41c-e01cd687a7a4.JPG)

This robot code is made for Reuseum Educational Inc. Boxy the Robot

This robot required the materials below:
- 1x esp8266 NODEMCU v1
- 1x l9110s motor driver
- 2x geared DC motors
- 1x 8.4v 18650 holder
- 2x 18650 batteries
- 1x custom power breakout board

This code is for a 2wd robot made out of a box and an esp8266 NODEMCU v1. It uses virtualjoystick from https://github.com/jeromeetienne/virtualjoystick.js to control the robot's movement via a web interface.

For the robot build, we will be pushing the code via the Arduino IDE and will therefore require the ESP8266 libraries for the Arduino. The following link will guide you with the installation of the librariers for your ESP. https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/

The robot has two motors, Motor A and Motor B. These motors are connected to pin D5 and pin D1 respectively, which control the speed of the motors. The direction of 
the motors are controlled by pin D6 and pin D2 respectively. The robot also has an LED variable built onto the esp LED.

The main function of this code is to control the motors and LED based on the input from the joystick and slider on the web interface. The ESP will ping out its own wifi to connect to and will be named based on the ssid name in the code. The IP created is what must be connected to in order to control the robot via a smart device.

- The toggleLED() function toggles the state of the LED by inverting its current state. 
- The getLEDState() function retrieves the current state of the LED and sends it back to the web interface. 
- The handleJSData() function receives the x and y coordinates of the joystick from the web interface and uses them to control the speed and direction of the motors. 
- The getslider() function retrieves the value of the slider from the web interface and stores it as the maximum motor speed. 
- The updateSliderPWM() function updates the value of the maximum motor speed based on the position of the slider and sends it to the server.
- The setup() function initializes the pin modes and starts the WiFi connection. 
- The loop() function continually listens for incoming HTTP requests and sends them to the appropriate functions to be handled.
