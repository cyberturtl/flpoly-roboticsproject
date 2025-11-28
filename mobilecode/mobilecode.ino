#include <SoftwareSerial.h>
#include <Servo.h>

Servo servo01;
Servo servo02;
Servo servo03;
Servo servo04;
Servo servo05;
Servo servo06;

SoftwareSerial Bluetooth(3, 4); // Arduino(RX, TX) - HC-05 Bluetooth (TX, RX)

int servo1Pos, servo2Pos, servo3Pos, servo4Pos, servo5Pos, servo6Pos; // current position
int servo1PPos, servo2PPos, servo3PPos, servo4PPos, servo5PPos, servo6PPos; // previous position
int servo01SP[50], servo02SP[50], servo03SP[50], servo04SP[50], servo05SP[50], servo06SP[50]; // for storing positions/steps
int speedDelay = 20;
int index = 0;
String dataIn = "";
unsigned long lastServoCmdTime = 0;
int pendingServoID = 0;
int pendingServoPos = -1;

void setup() {
  servo01.attach(5); // waist servo
  servo02.attach(6); // shoulder servo 
  servo03.attach(7); // elbow servo
  servo04.attach(8); // wrist (roll) servo
  servo05.attach(9); // wrist (pitch) servo
  servo06.attach(10); // gripper servo
  Bluetooth.begin(9600); // Default baud rate of the Bluetooth module
  Bluetooth.setTimeout(1);
  delay(20);
  // Robot arm initial position
  servo1PPos = 40; // default = 90
  servo01.write(servo1PPos);
  
  // higher number raises it up
  servo2PPos = 130; // default = 150
  servo02.write(servo2PPos);
  
  // lower number raises it up
  servo3PPos = 85; // default = 35
  servo03.write(servo3PPos);
  
  servo4PPos = 90; // default = 140
  servo04.write(servo4PPos);
  
  // lower number pitches up
  servo5PPos = 70; // default = 85
  servo05.write(servo5PPos);
  
  //lower number closes
  servo6PPos = 130; // default = 80
  servo06.write(servo6PPos);
}

void loop() {

  // Read packet
  if (Bluetooth.available()) {
    dataIn = Bluetooth.readStringUntil('#');
    lastServoCmdTime = millis();  // time new data arrived
  }

  // ----- HANDLE SERVO COMMANDS -----
  if (dataIn.startsWith("s")) {
    pendingServoID  = dataIn.substring(1, 2).toInt();
    pendingServoPos = dataIn.substring(2).toInt();
    dataIn = "";
  }

  // ----- MOVE SERVO ONLY AFTER INPUT STOPS -----
  if (pendingServoID != 0 && (millis() - lastServoCmdTime > 40)) {

    int* prev;
    Servo* s;

    switch (pendingServoID) {
      case 1: prev = &servo1PPos; s = &servo01; break;
      case 2: prev = &servo2PPos; s = &servo02; break;
      case 3: prev = &servo3PPos; s = &servo03; break;
      case 4: prev = &servo4PPos; s = &servo04; break;
      case 5: prev = &servo5PPos; s = &servo05; break;
      case 6: prev = &servo6PPos; s = &servo06; break;
    }

    if (pendingServoPos != *prev) {
      s->write(pendingServoPos);
      *prev = pendingServoPos;
    }

    // Clear pending command
    pendingServoID = 0;
    pendingServoPos = -1;
  }


  // ----- SAVE -----
  if (dataIn.startsWith("SAVE")) {
    servo01SP[index] = servo1PPos;
    servo02SP[index] = servo2PPos;
    servo03SP[index] = servo3PPos;
    servo04SP[index] = servo4PPos;
    servo05SP[index] = servo5PPos;
    servo06SP[index] = servo6PPos;
    index++;
    dataIn = "";
    return;
  }

  // ----- RUN -----
  if (dataIn.startsWith("RUN")) {
    dataIn = "";
    runservo();
    return;
  }

  // ----- RESET -----
  if (dataIn == "RESET") {
    memset(servo01SP, 0, sizeof(servo01SP));
    memset(servo02SP, 0, sizeof(servo02SP));
    memset(servo03SP, 0, sizeof(servo03SP));
    memset(servo04SP, 0, sizeof(servo04SP));
    memset(servo05SP, 0, sizeof(servo05SP));
    memset(servo06SP, 0, sizeof(servo06SP));
    index = 0;
    dataIn = "";
    return;
  }
}

// Automatic mode custom function - run the saved steps
void runservo() {
  while (dataIn != "RESET") {   // Run the steps over and over again until "RESET" button is pressed
    for (int i = 0; i <= index - 2; i++) {  // Run through all steps(index)
      if (Bluetooth.available() > 0) {      // Check for incomding data
        dataIn = Bluetooth.readString();
        if ( dataIn == "PAUSE") {           // If button "PAUSE" is pressed
          while (dataIn != "RUN") {         // Wait until "RUN" is pressed again
            if (Bluetooth.available() > 0) {
              dataIn = Bluetooth.readString();
              if ( dataIn == "RESET") {     
                break;
              }
            }
          }
        }
        // If speed slider is changed
        if (dataIn.startsWith("ss")) {
          String dataInS = dataIn.substring(2, dataIn.length());
          speedDelay = dataInS.toInt(); // Change servo speed (delay time)
        }
      }
      // Servo 1
      if (servo01SP[i] == servo01SP[i + 1]) {
      }
      if (servo01SP[i] > servo01SP[i + 1]) {
        for ( int j = servo01SP[i]; j >= servo01SP[i + 1]; j--) {
          servo01.write(j);
          delay(speedDelay);
        }
      }
      if (servo01SP[i] < servo01SP[i + 1]) {
        for ( int j = servo01SP[i]; j <= servo01SP[i + 1]; j++) {
          servo01.write(j);
          delay(speedDelay);
        }
      }

      // Servo 2
      if (servo02SP[i] == servo02SP[i + 1]) {
      }
      if (servo02SP[i] > servo02SP[i + 1]) {
        for ( int j = servo02SP[i]; j >= servo02SP[i + 1]; j--) {
          servo02.write(j);
          delay(speedDelay);
        }
      }
      if (servo02SP[i] < servo02SP[i + 1]) {
        for ( int j = servo02SP[i]; j <= servo02SP[i + 1]; j++) {
          servo02.write(j);
          delay(speedDelay);
        }
      }

      // Servo 3
      if (servo03SP[i] == servo03SP[i + 1]) {
      }
      if (servo03SP[i] > servo03SP[i + 1]) {
        for ( int j = servo03SP[i]; j >= servo03SP[i + 1]; j--) {
          servo03.write(j);
          delay(speedDelay);
        }
      }
      if (servo03SP[i] < servo03SP[i + 1]) {
        for ( int j = servo03SP[i]; j <= servo03SP[i + 1]; j++) {
          servo03.write(j);
          delay(speedDelay);
        }
      }

      // Servo 4
      if (servo04SP[i] == servo04SP[i + 1]) {
      }
      if (servo04SP[i] > servo04SP[i + 1]) {
        for ( int j = servo04SP[i]; j >= servo04SP[i + 1]; j--) {
          servo04.write(j);
          delay(speedDelay);
        }
      }
      if (servo04SP[i] < servo04SP[i + 1]) {
        for ( int j = servo04SP[i]; j <= servo04SP[i + 1]; j++) {
          servo04.write(j);
          delay(speedDelay);
        }
      }

      // Servo 5
      if (servo05SP[i] == servo05SP[i + 1]) {
      }
      if (servo05SP[i] > servo05SP[i + 1]) {
        for ( int j = servo05SP[i]; j >= servo05SP[i + 1]; j--) {
          servo05.write(j);
          delay(speedDelay);
        }
      }
      if (servo05SP[i] < servo05SP[i + 1]) {
        for ( int j = servo05SP[i]; j <= servo05SP[i + 1]; j++) {
          servo05.write(j);
          delay(speedDelay);
        }
      }

      // Servo 6
      if (servo06SP[i] == servo06SP[i + 1]) {
      }
      if (servo06SP[i] > servo06SP[i + 1]) {
        for ( int j = servo06SP[i]; j >= servo06SP[i + 1]; j--) {
          servo06.write(j);
          delay(speedDelay);
        }
      }
      if (servo06SP[i] < servo06SP[i + 1]) {
        for ( int j = servo06SP[i]; j <= servo06SP[i + 1]; j++) {
          servo06.write(j);
          delay(speedDelay);
        }
      }
    }
  }
}