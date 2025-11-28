#include <Servo.h>

// define the 6 servos connected to the arduino
Servo servo01;
Servo servo02;
Servo servo03;
Servo servo04;
Servo servo05;
Servo servo06;

// SPEED SETTING: Higher number = Slower movement
// 10 is fast, 50 is slow
// this one only applies to the startup procedure 
int moveSpeed = 10;

void setup() {
  // pre set positions
  servo01.write(40);   // Waist start pos
  servo02.write(160);  // Shoulder start pos
  servo03.write(45);   // Elbow start pos
  servo04.write(90);   // Wrist Roll start pos
  servo05.write(30);   // Wrist Pitch start pos
  servo06.write(130);  // Gripper start pos
  
  // "attach" the servos to the arduino
  //servo01.attach(5);
  servo01.attach(5, 500, 2700); // attaches differently to avoid the software limitation with 'servo.h' library
  servo02.attach(6); 
  servo03.attach(7); 
  servo04.attach(8); 
  servo05.attach(9); 
  servo06.attach(10); 
  
  // Wait a moment for the motors to stiffen up
  delay(1000);
}

void loop() {
  // position to duck
  slowMove(servo01, 40); // Move waist to 40
  slowMove(servo05, 70); // Wrist pitch to 70
  slowMove(servo02, 130); // Move shoulder to 130
  slowMove(servo03, 85); // Elbow to 85
  servo06.write(130); // Open Gripper (130)
  delay(1000); // Pause before grabbing

  // grab duck
  servo06.write(70); // gripper
  delay(1000);

  // lift duck
  slowMove(servo02, 180); // Shoulder up
  slowMove(servo05, 130); // Wrist adjust
  slowMove(servo03, 55);  // Elbow adjust

  // turn around
  slowMove(servo01, 180);

  // drop duck
  servo06.write(130); // gripper
  delay(2000);
}

// function to move servos slowly
void slowMove(Servo &myServo, int targetPos) {
  int startPos = myServo.read(); // Get current position

  // If we need to move UP (e.g. 0 to 90)
  if (startPos < targetPos) {
    for (int i = startPos; i <= targetPos; i++) {
      myServo.write(i);
      delay(moveSpeed); 
    }
  }
  // If we need to move DOWN (e.g. 90 to 0)
  else {
    for (int i = startPos; i >= targetPos; i--) {
      myServo.write(i);
      delay(moveSpeed); 
    }
  }
}

// position to grab duck is (before closing gripper)
  // s1 = 40
  // s2 = 130
  // s3 = 85
  // s4 = 90
  // s5 = 70
  // s6 = 130