#include "Brain.h"
#include "Config.h"
#include "Drive.h"
#include "IRSensors.h"
#include "TurnLevelsTest.h"
#include "TurnLevels.h"

enum Mode : uint8_t { MODE_MANUAL, MODE_AUTO };
static Mode mode = MODE_MANUAL;

enum AutoState : uint8_t { AUTO_IDLE, AUTO_STAGE1, AUTO_DONE };
static AutoState autoState = AUTO_IDLE;

static unsigned long autoStartMs = 0;

static void autoEnter() {
  autoStartMs = millis();
  autoState = AUTO_STAGE1;
  Serial.println("Beginning linefollow");
}

// ---- Python ReadingMove dict -> C++ mapping ----
static TurnLevel readingToMove(uint8_t L, uint8_t C, uint8_t R) {
  // Convert tuple (L,C,R) to bits: LCR
  uint8_t bits = ((L & 1) << 2) | ((C & 1) << 1) | (R & 1);

  switch (bits) {
    case 0b000: return TurnLevel::TURN_RIGHT;  // (0,0,0)
    case 0b100: return TurnLevel::TURN_RIGHT;   // (1,0,0)
    case 0b110: return TurnLevel::VEER_RIGHT;  // (1,1,0)
    case 0b111: return TurnLevel::SPIN_RIGHT;    // (1,1,1)
    case 0b011: return TurnLevel::VEER_LEFT; // (0,1,1)
    case 0b001: return TurnLevel::TURN_LEFT;  // (0,0,1)
    case 0b101: return TurnLevel::STRAIGHT;    // (1,0,1)
    case 0b010: return TurnLevel::STRAIGHT;    // (0,1,0)
    default:    return TurnLevel::SPIN_RIGHT;  // fallback
  }
}

static bool autoUpdate() {
  // Safety timeout
  if (millis() - autoStartMs > AUTO_TIMEOUT_MS) {
    autoState = AUTO_DONE;
    Serial.println("AUTO timeout -> stopping, flip switch off/on to restart.");
  }

  switch (autoState) {
    case AUTO_STAGE1: {
        
      // Example: drive forward for 2 seconds
    
      TurnLevel move = readingToMove(L, C, R);    
      TurnLevels_apply(move);
    //   Drive_stopAll();
      return false;
    }

    case AUTO_DONE:
      Drive_stopAll();
      return true;

    default:
      return true;
  }

  return false;
}

void Brain_init() {
  // nothing yet
}

void Brain_update() {
  // Update sensors/hardware inputs first
  Drive_updateIBus();
  IRSensors_update();

  if (!Drive_ibusFrameOK()) {
    Drive_stopAll();
    return;
  }

  bool autoSwitch = Drive_readAutoSwitch();

  // Mode transitions
  if (mode == MODE_MANUAL && autoSwitch) {
    mode = MODE_AUTO;
    autoEnter();
  } else if (mode == MODE_AUTO && !autoSwitch) {
    mode = MODE_MANUAL;
  }

  // Execute current mode
  if (mode == MODE_MANUAL) {
    Drive_manualUpdate();
  } else {
    if (autoUpdate()) {
      mode = MODE_MANUAL; // fall back after completion
      Serial.println("AUTO complete -> returning to MANUAL."); //maybe I shouldn't do this
    }
  }
}
