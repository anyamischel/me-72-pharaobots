// This code bridges the gap between autonomous and manual. Channel 5 is used to switch between the two modes.
// as of 2/16/26 Same as drivewauto2.cpp however I just tried to clean up some if statements, but honestly i didn't clean it up that much


// Include necessary libraries
#include <IBusBM.h>
#include "RoboClaw.h"
#include <Servo.h>


// Create necessary global objects
IBusBM ibus; // ibus object is a decoder
RoboClaw roboclaw1(&Serial1, 10000); // create 2 roboclaw objects, one for each serial port its connected to
RoboClaw roboclaw2(&Serial3, 10000); // the 10000 refers to microseconds to wait for connection before proceeding




#define ROBOCLAW_ADDRESS 0x80 // defines some text as values that may or may not be needed but; helps with clarity


// Channel Values for driving
int rcCH1 = 0;   // Left - Right
int rcCH2 = 0;   // Forward - Reverse
int rcCH3 = 0;   // Acceleration
int rcCH4 = 0; // autonomous in/out switch
bool rcCH8 = 0;   // Switch between drive and climb mode
int rcCH9 = 0; // Intake switch

const int trexPin = 5;     // Arduino pin that outputs the servo pulse to TReX
Servo trexCmd;

// Intake speed
const int INTAKE_STOP_US   = 1500;  // neutral/stop in RC-style systems
const int INTAKE_SPEED1_US = 1600;  // mild forward
const int INTAKE_SPEED2_US = 1700;  // faster forward

////
// For autonomous
const byte AUTO_SWITCH_CH = 4;
const bool AUTO_SWITCH_DEFAULT = false; // will always stay false
unsigned long autoStartMs = 0;
const unsigned long AUTO_TIMEOUT_MS = 60000; //40 seconds




// ---- iBus helpers (unchanged idea) ----
int readChannel(byte channelInput, int minLimit, int maxLimit, int defaultValue) {
  uint16_t ch = ibus.readChannel(channelInput); // note that the readChannel used here is a method of the ibus object, not the function that we are defining.
  if (ch < 100) return defaultValue; // returns a neutral value assuming some noise is present
  return map(ch, 1000, 2000, minLimit, maxLimit); // maps the traditional RC values to a new set of minLimit and maxLimit values that are better read by roboclaw
}




bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = defaultValue ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}


enum MotorChan : uint8_t { M1 = 1, M2 = 2 };


void driveMotorCmd(RoboClaw &rc, uint8_t chan, int cmd) { // cmd ∈ [-127,127] driveMotorCmd(roboclaw1, M1, rightCmd)
  cmd = constrain(cmd, -127, 127);
  uint8_t sp = (uint8_t)abs(cmd); // sp stands for speed
  if (chan == M1) {
    if (cmd >= 0) rc.ForwardM1( ROBOCLAW_ADDRESS, sp); // the purpose for this entire section is that roboclaw can only read absolute values and only distinguishes
    else          rc.BackwardM1(ROBOCLAW_ADDRESS, sp); // forward and backward through direct methods (rc.ForwardM1)
  } else { // M2
    if (cmd >= 0) rc.ForwardM2(ROBOCLAW_ADDRESS, sp);
    else          rc.BackwardM2(ROBOCLAW_ADDRESS, sp);
  }
}


enum Mode : uint8_t { MODE_MANUAL, MODE_AUTO};


Mode mode = MODE_MANUAL;


enum AutoState : uint8_t { AUTO_IDLE, AUTO_STAGE1, AUTO_DONE };
AutoState autoState = AUTO_IDLE;


void autoEnter() {
  autoStartMs = millis();
  autoState = AUTO_STAGE1;
}


bool autoUpdate() {
  // Hard timeout safety
  if (millis() - autoStartMs > AUTO_TIMEOUT_MS) { //if auto exceeds 40 seconds it exits
    autoState = AUTO_DONE;
    Serial.println("autonomous timed out, please re-set switch 5 on controller.");
  }


  switch (autoState) {
    case AUTO_STAGE1:
      // Example: drive forward for 2 seconds
      if (millis() - autoStartMs < 2000) {
        // forward command example
        int cmd = 60;
        Serial.println("driving forward on auto");
        driveMotorCmd(roboclaw1, M1, cmd);
        driveMotorCmd(roboclaw1, M2, cmd);
        // driveMotorCmd(roboclaw2, M1, cmd);
        // driveMotorCmd(roboclaw2, M2, cmd);
      } else {
        autoState = AUTO_DONE;
        return true;
      }
      break;
    case AUTO_DONE:
      // stop and report completion
      driveMotorCmd(roboclaw1, M1, 0);
      driveMotorCmd(roboclaw1, M2, 0);
      driveMotorCmd(roboclaw2, M1, 0);
      driveMotorCmd(roboclaw2, M2, 0);
      return true;


    default:
      return true;
  }


  return false;
}




//////////// our set up simply opens up communications between the arduino & receiver and the arduino & roboclaw
//////////// each one will need to use the objects of their specific classes based on the imported libraries
//////////// the code also includes some led checks (likely not necessary) and then ensures that on start up all the motors are stopped


const int stopCmd = 0;


void setup() {
  Serial.begin(115200);


  // iBus is on pin 17 (RX2) => Serial2
  Serial2.begin(115200);
  ibus.begin(Serial2);




  // RoboClaw on Serial1 (pins 18/19)
  Serial1.begin(38400);
  roboclaw1.begin(38400);
 
  Serial3.begin(38400);
  roboclaw2.begin(38400);


  // Stop motors
 
  driveMotorCmd(roboclaw1, M1, stopCmd);
  driveMotorCmd(roboclaw1, M2, stopCmd);
  driveMotorCmd(roboclaw2, M1, stopCmd);
  driveMotorCmd(roboclaw2, M2, stopCmd);
 
    ///////////// Intake Motor Set Up
 
   // ---- TReX setup ----
  trexCmd.attach(trexPin);
  trexCmd.writeMicroseconds(INTAKE_STOP_US); // stop at startup
}


void manualUpdate() {
 // Read RC channels (same indices as your example)
  rcCH1 = readChannel(0, -100, 100, 0);
  rcCH2 = readChannel(1, -100, 100, 0);
  rcCH3 = readChannel(2, 0,   100, 0);   // acceleration base (0..100)
  rcCH8 = readSwitch(7, false);


  if (!rcCH8) {
  // Throttle limit from CH3: 0..100 -> 0..127
  int maxMag = map(rcCH3, 0, 100, 0, 127);
 
  // Forward/back command from CH2: signed in [-maxMag, +maxMag]
  int fwd = map(abs(rcCH2), 0, 100, 0, maxMag);
  if (rcCH2 < 0) fwd = -fwd;
 
  // Steering command from CH1: signed in [-maxMag, +maxMag]
  // (If steering is too strong, use a smaller range like -40..40 mapped to maxMag)
  int turn = map(rcCH1, -100, 100, -maxMag, maxMag);
 
  // Differential mix
  int leftCmd  = fwd + turn;
  int rightCmd = fwd - turn;
 
  // Clamp to RoboClaw command range
  leftCmd  = constrain(leftCmd,  -127, 127);
  rightCmd = constrain(rightCmd, -127, 127);
 
  // Deadband to avoid creep
  if (abs(leftCmd)  < 3) leftCmd  = 0;
  if (abs(rightCmd) < 3) rightCmd = 0;
 
  // OPTIONAL: per-side inversion flags (very common with mirrored drivetrains)
  const bool INVERT_LEFT  = false;
  const bool INVERT_RIGHT = false;
  if (INVERT_LEFT)  leftCmd  = -leftCmd;
  if (INVERT_RIGHT) rightCmd = -rightCmd;
 
  // APPLY TO YOUR WIRING:
  // RoboClaw1 = both RIGHT motors
  driveMotorCmd(roboclaw1, M1, rightCmd);
  driveMotorCmd(roboclaw1, M2, rightCmd);
 
  // RoboClaw2 = both LEFT motors
  driveMotorCmd(roboclaw2, M1, leftCmd);
  driveMotorCmd(roboclaw2, M2, leftCmd);
  } else {
    /// CH3: forward cruise speed
  int speedMag = map(rcCH3, 0, 100, 0, 127);
 
  // CH2 speed adjustments
  int adj = map(abs(rcCH2), 0, 100, 0, 127);
  if (rcCH2 < 0) adj = -adj;

  int fwd = -(adj + speedMag);
  fwd = constrain(fwd, -127, 127);


 
  // Steering command from CH1: signed in [-maxMag, +maxMag]
  // (If steering is too strong, use a smaller range like -40..40 mapped to maxMag)
  const float TURN_GAIN = 0.6;
  int turnMax = (int)(TURN_GAIN * 127);
  int turn = map(rcCH1, -100, 100, -turnMax, turnMax);
 
  // Differential mix for driving backwards
  int leftCmd  = fwd - turn;
  int rightCmd = fwd + turn;
 
  // Clamp to RoboClaw command range
  leftCmd  = constrain(leftCmd,  -127, 127);
  rightCmd = constrain(rightCmd, -127, 127);
 
  // Deadband to avoid creep
  if (abs(leftCmd)  < 3) leftCmd  = 0;
  if (abs(rightCmd) < 3) rightCmd = 0;
 
  // OPTIONAL: per-side inversion flags (very common with mirrored drivetrains)
  const bool INVERT_LEFT  = false;
  const bool INVERT_RIGHT = false;
  if (INVERT_LEFT)  leftCmd  = -leftCmd;
  if (INVERT_RIGHT) rightCmd = -rightCmd;
 
  // APPLY TO YOUR WIRING:
  // RoboClaw1 = both RIGHT motors
  driveMotorCmd(roboclaw1, M1, rightCmd);
  driveMotorCmd(roboclaw1, M2, rightCmd);
 
  // RoboClaw2 = both LEFT motors
  driveMotorCmd(roboclaw2, M1, leftCmd);
  driveMotorCmd(roboclaw2, M2, leftCmd);
  }
  


  // Input Motor Control
  rcCH9 = readChannel(8, -100, 100, 0);
  int intsp = map(rcCH9, -100, 100, 1000, 2000);
    // Clamp to TRex range?
  intsp  = constrain(intsp,  1000, 2000);
  // deadpand?
  if (abs(intsp - INTAKE_STOP_US)  < 40) intsp = INTAKE_STOP_US;
  trexCmd.writeMicroseconds(intsp);


}




void loop() {
  ibus.loop(); // mandatory, reads byte arriving on serial2
  bool ibusOK = (ibus.readChannel(0) >= 100);  // crude but effective "frame present" check
  if (!ibusOK) {
    driveMotorCmd(roboclaw1, M1, stopCmd);
    driveMotorCmd(roboclaw1, M2, stopCmd);
    driveMotorCmd(roboclaw2, M1, stopCmd);
    driveMotorCmd(roboclaw2, M2, stopCmd);
    trexCmd.writeMicroseconds(INTAKE_STOP_US);
 
    return;
  }


  
  
  bool autoSwitch = readSwitch(AUTO_SWITCH_CH, AUTO_SWITCH_DEFAULT);


    // Mode transitions
  if (mode == MODE_MANUAL && autoSwitch) {
    mode = MODE_AUTO;
    autoEnter();
  } 
  else if (mode == MODE_AUTO && !autoSwitch) {
    // manual override
    mode = MODE_MANUAL;
    manualUpdate();
  }


  else if (mode == MODE_MANUAL) {
    manualUpdate();
  } else {


    if (autoUpdate();) {
      mode = MODE_MANUAL; // fall back after completion
      manualUpdate();
      Serial.println("i am here and going back to manual.");
    }


    // Decide intake behavior during auto:
    // either keep it off or run with some autonomous logic.
    analogWrite(enA, 0);
  }
  delay(20);
}
