![DSC02828](https://user-images.githubusercontent.com/121644329/210115160-8a283f81-5afd-439f-b41c-e01cd687a7a4.JPG)

This robot code is made for Reuseum Educational boxy the robot
this code usses: 
esp8266 nodemcu v1
l9110s motor driver
Two geared dc motors
8.4v 18650 holder
two 18650 batteries
custom made power breakout 

/* This code is for a 2wd robot made out of a box and an esp8266 nodmcu v1. It uses
 virtualjoystick from" https://github.com/jeromeetienne/virtualjoystick.js" to control the robot's movement via a web interface.

The robot has two motors, Motor A and Motor B. These motors are connected to pins 
pin d5, and pin d1 respectively, which control the speed of the motors. The direction of
 the motors is controlled by the pin d6 and pin d2 , respectively. The robot also has 
an LED which is connected to built in esp led.

The main function of this code is to control the motors and LED based on input from the 
joystick and slider on the web interface. The toggleLED() function toggles the state of
 the LED by inverting its current state. The getLEDState() function retrieves the current 
state of the LED and sends it back to the web interface. The handleJSData() function receives
 the x and y coordinates of the joystick from the web interface and uses them to control the 
speed and direction of the motors. The getslider() function retrieves the value of the slider
 from the web interface and stores it as the maximum motor speed. The updateSliderPWM() function 
updates the value of the maximum motor speed based on the position of the slider and sends it to the server.

The setup() function initializes the pin modes and starts the WiFi connection. The loop() function
 continually listens for incoming HTTP requests and sends them to the appropriate functions to be handled.
*/
