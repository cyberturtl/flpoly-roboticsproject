#include <Servo.h>

// Note: SoftwareSerial is REMOVED. 
// We are using Hardware Serial (Pins 0 and 1) for smoother performance.

Servo servo01;
Servo servo02;
Servo servo03;
Servo servo04;
Servo servo05;
Servo servo06;

// Variables
int servo1Pos, servo2Pos, servo3Pos, servo4Pos, servo5Pos, servo6Pos; 
int servo1PPos, servo2PPos, servo3PPos, servo4PPos, servo5PPos, servo6PPos; 
int servo01SP[50], servo02SP[50], servo03SP[50], servo04SP[50], servo05SP[50], servo06SP[50]; 
int speedDelay = 20;
int index = 0;
String dataIn = "";
unsigned long lastServoCmdTime = 0;
int pendingServoID = 0;
int pendingServoPos = -1;

void setup() {
  // PRE-SET POSITIONS (Fixes the startup slam)
  servo1PPos = 40;  
  servo01.write(servo1PPos);
  
  servo2PPos = 160; 
  servo02.write(servo2PPos);
  
  servo3PPos = 45; 
  servo03.write(servo3PPos);
  
  servo4PPos = 90; 
  servo04.write(servo4PPos);
  
  servo5PPos = 30; 
  servo05.write(servo5PPos);
  
  servo6PPos = 130; 
  servo06.write(servo6PPos);

  // START SERIAL (Hardware Serial on Pins 0 & 1)
  // IMPORTANT: Unplug Bluetooth RX/TX when uploading this code!
  Serial.begin(9600); 
  Serial.setTimeout(5); // Reduced timeout for faster response

  delay(20);

  // ATTACH SERVOS
  // (Using extended range on Waist if you want that extra reach)
  servo01.attach(5, 500, 2600); 
  servo02.attach(6); 
  servo03.attach(7); 
  servo04.attach(8); 
  servo05.attach(9); 
  servo06.attach(10); 
}

void loop() {
  // ----- READ DATA (Using Hardware Serial) -----
  if (Serial.available()) {
    dataIn = Serial.readStringUntil('#'); // Reads until '#' or timeout
    lastServoCmdTime = millis();  
  }

  // ----- HANDLE SERVO COMMANDS -----
  if (dataIn.startsWith("s")) {
    pendingServoID  = dataIn.substring(1, 2).toInt();
    pendingServoPos = dataIn.substring(2).toInt();
    dataIn = ""; // Clear buffer
  }

  // ----- EXECUTE MOVE -----
  // We wait 40ms after command to ensure no more data is flooding in
  if (pendingServoID != 0 && (millis() - lastServoCmdTime > 40)) {

    int* prev;
    Servo* s;

    // Pointer assignment
    switch (pendingServoID) {
      case 1: prev = &servo1PPos; s = &servo01; break;
      case 2: prev = &servo2PPos; s = &servo02; break;
      case 3: prev = &servo3PPos; s = &servo03; break;
      case 4: prev = &servo4PPos; s = &servo04; break;
      case 5: prev = &servo5PPos; s = &servo05; break;
      case 6: prev = &servo6PPos; s = &servo06; break;
      default: pendingServoID = 0; return; // Invalid ID check
    }

    // SAFETY CHECK: Fix erratic movements
    // Sometimes Bluetooth drops a digit (e.g., "180" becomes "18").
    // We ignore big, instant jumps if they look suspicious, or just clamp the value.
    if (pendingServoPos < 0) pendingServoPos = 0;
    if (pendingServoPos > 180) pendingServoPos = 180;

    // Perform the move
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
    // Check array bounds to prevent crashing
    if(index < 50) {
        servo01SP[index] = servo1PPos;
        servo02SP[index] = servo2PPos;
        servo03SP[index] = servo3PPos;
        servo04SP[index] = servo4PPos;
        servo05SP[index] = servo5PPos;
        servo06SP[index] = servo6PPos;
        index++;
    }
    dataIn = "";
  }

  // ----- RUN -----
  if (dataIn.startsWith("RUN")) {
    dataIn = "";
    runservo();
  }

  // ----- RESET -----
  if (dataIn == "RESET") {
    // Standard way to clear arrays
    memset(servo01SP, 0, sizeof(servo01SP));
    memset(servo02SP, 0, sizeof(servo02SP));
    memset(servo03SP, 0, sizeof(servo03SP));
    memset(servo04SP, 0, sizeof(servo04SP));
    memset(servo05SP, 0, sizeof(servo05SP));
    memset(servo06SP, 0, sizeof(servo06SP));
    index = 0;
    dataIn = "";
  }
}

// Automatic mode custom function
void runservo() {
  while (dataIn != "RESET") {   
    for (int i = 0; i <= index - 2; i++) {
      if (Serial.available() > 0) {      
        dataIn = Serial.readString();
        
        // PAUSE LOGIC
        if (dataIn == "PAUSE") {           
          while (dataIn != "RUN") {         
            if (Serial.available() > 0) {
              dataIn = Serial.readString();
              if (dataIn == "RESET") break;
            }
          }
        }
        
        // SPEED SLIDER
        if (dataIn.startsWith("ss")) {
          String dataInS = dataIn.substring(2, dataIn.length());
          speedDelay = dataInS.toInt(); 
        }
      }

      // Execute Moves using Helper Function to save space/cleanliness
      moveStoredServo(servo01, servo01SP, i);
      moveStoredServo(servo02, servo02SP, i);
      moveStoredServo(servo03, servo03SP, i);
      moveStoredServo(servo04, servo04SP, i);
      moveStoredServo(servo05, servo05SP, i);
      moveStoredServo(servo06, servo06SP, i);
    }
  }
}

// Helper function to clean up the big "runservo" loop
void moveStoredServo(Servo &s, int sp[], int i) {
    if (sp[i] == sp[i+1]) return; // No move needed

    if (sp[i] > sp[i+1]) {
        for (int j = sp[i]; j >= sp[i+1]; j--) {
            s.write(j);
            delay(speedDelay);
        }
    } else {
        for (int j = sp[i]; j <= sp[i+1]; j++) {
            s.write(j);
            delay(speedDelay);
        }
    }
}