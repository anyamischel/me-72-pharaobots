#include "Drive.h"
#include "Config.h"

#include <IBusBM.h>
#include "RoboClaw.h"
#include <Servo.h>

// -------------------- Hardware objects --------------------
static IBusBM ibus;
static RoboClaw roboclawRight(&Serial1, 10000); // RoboClaw1 = RIGHT motors
static RoboClaw roboclawLeft (&Serial3, 10000); // RoboClaw2 = LEFT motors
static Servo trexCmd;

// -------------------- Helpers --------------------
static int readChannel(uint8_t ch, int minLimit, int maxLimit, int defaultValue) {
  uint16_t raw = ibus.readChannel(ch);
  if (raw < 100) return defaultValue;
  return map(raw, 1000, 2000, minLimit, maxLimit);
}

static bool readSwitch(uint8_t ch, bool defaultValue) {
  int intDefault = defaultValue ? 100 : 0;
  int v = readChannel(ch, 0, 100, intDefault);
  return (v > 50);
}

enum MotorChan : uint8_t { M1 = 1, M2 = 2 };

static void driveMotorCmd(RoboClaw &rc, uint8_t chan, int cmd) {
  cmd = constrain(cmd, -127, 127);
  uint8_t sp = (uint8_t)abs(cmd);
  if (chan == M1) {
    if (cmd >= 0) rc.ForwardM1( ROBOCLAW_ADDRESS, sp);
    else          rc.BackwardM1(ROBOCLAW_ADDRESS, sp);
  } else {
    if (cmd >= 0) rc.ForwardM2( ROBOCLAW_ADDRESS, sp);
    else          rc.BackwardM2(ROBOCLAW_ADDRESS, sp);
  }
}

// Public low-level
void Drive_setLeftRight(int leftCmd, int rightCmd) {
  leftCmd  = constrain(leftCmd,  -127, 127);
  rightCmd = constrain(rightCmd, -127, 127);

  if (abs(leftCmd)  < DRIVE_DEADBAND) leftCmd = 0;
  if (abs(rightCmd) < DRIVE_DEADBAND) rightCmd = 0;

  // RIGHT side motors (RoboClaw on Serial1)
  driveMotorCmd(roboclawRight, M1, rightCmd);
  driveMotorCmd(roboclawRight, M2, rightCmd);

  // LEFT side motors (RoboClaw on Serial3)
  driveMotorCmd(roboclawLeft,  M1, leftCmd);
  driveMotorCmd(roboclawLeft,  M2, leftCmd);
}

void Drive_stopAll() {
  Drive_setLeftRight(0, 0);
  Drive_setIntakeMicroseconds(INTAKE_STOP_US);
}

void Drive_setIntakeMicroseconds(int us) {
  us = constrain(us, INTAKE_MIN_US, INTAKE_MAX_US);
  if (abs(us - INTAKE_STOP_US) < 40) us = INTAKE_STOP_US; // deadband around stop
  trexCmd.writeMicroseconds(us);
}

void Drive_init() {
  Serial.begin(DEBUG_BAUD);

  // iBus on Serial2
  Serial2.begin(IBUS_BAUD);
  ibus.begin(Serial2);

  // RoboClaw serials
  Serial1.begin(ROBOCLAW_BAUD);
  roboclawRight.begin(ROBOCLAW_BAUD);

  Serial3.begin(ROBOCLAW_BAUD);
  roboclawLeft.begin(ROBOCLAW_BAUD);

  // Intake servo
  trexCmd.attach(TREC_CMD_PIN);
  trexCmd.writeMicroseconds(INTAKE_STOP_US);

  Drive_stopAll();
}

void Drive_updateIBus() {
  ibus.loop();
}

bool Drive_ibusFrameOK() {
  return (ibus.readChannel(CH_STEER) >= 100); // your same “frame present” check
}

bool Drive_readAutoSwitch() {
  return readSwitch(CH_AUTO_SW, AUTO_SWITCH_DEFAULT);
}

void Drive_manualUpdate() {
  // Read channels
  int chSteer  = readChannel(CH_STEER,    -100, 100, 0);
  int chThrot  = readChannel(CH_THROTTLE, -100, 100, 0);
  int chAccel  = readChannel(CH_ACCEL,      0, 100, 0);
  bool climbMode = readSwitch(CH_DRIVE_MODE, false);

  // Intake
  int chIntake = readChannel(CH_INTAKE, -100, 100, 0);
  int intakeUs = map(chIntake, -100, 100, INTAKE_MIN_US, INTAKE_MAX_US);
  Drive_setIntakeMicroseconds(intakeUs);

  // ---- Driving modes ----
  if (!climbMode) {
    // Normal drive: differential mix
    int maxMag = map(chAccel, 0, 100, 0, 127);

    int fwd = map(abs(chThrot), 0, 100, 0, maxMag);
    if (chThrot < 0) fwd = -fwd;

    int turn = map(chSteer, -100, 100, -maxMag, maxMag);

    int leftCmd  = fwd + turn;
    int rightCmd = fwd - turn;

    Drive_setLeftRight(leftCmd, rightCmd);
  } else {
    // Climb: your “backwards drive with steering gain” logic
    int speedMag = map(chAccel, 0, 100, 0, 127);

    int adj = map(abs(chThrot), 0, 100, 0, 127);
    if (chThrot < 0) adj = -adj;

    int fwd = -(adj + speedMag);
    fwd = constrain(fwd, -127, 127);

    const float TURN_GAIN = 0.6f;
    int turnMax = (int)(TURN_GAIN * 127);
    int turn = map(chSteer, -100, 100, -turnMax, turnMax);

    int leftCmd  = fwd - turn;
    int rightCmd = fwd + turn;

    Drive_setLeftRight(leftCmd, rightCmd);
  }
}
