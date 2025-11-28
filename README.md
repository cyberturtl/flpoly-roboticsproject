# flpoly-roboticsproject
for EEL4664, taught by Dr. Ngo, Hoan
contributed to by: Ryan Bezerra, Brian Scholes, William Denning

Our robotic arm was 3D printed using the design shared in this HowtoMechatronics link: https://howtomechatronics.com/tutorials/arduino/diy-arduino-robot-arm-with-smartphone-control/
The information listed out in that link also worked as a guide for contruction and setup of the project.
The author also developed an MIT App Inventor app to control the arm via an Android device, which we made use of.

# static code vs. mobile code
static code is a simple arduino script which maps the servos of the robot arm to a currated loop of movements for picking up the duck and placing it into a box on its opposing side.
mobile code is a an arduino script which takes advantage of the bluetooth capabilities of our setup and allows for control of the arm using the android app provided within the guide.
- both of these scripts are arduino scripts and show be direct uploads to an arduino board of your choice.
