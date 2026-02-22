#include "Brain.h"
#include "Config.h"
#include "Drive.h"
#include "IRSensors.h"

enum Mode : uint8_t { MODE_MANUAL, MODE_AUTO };
static Mode mode = MODE_MANUAL;

enum AutoState : uint8_t { AUTO_IDLE, AUTO_STAGE1, AUTO_DONE };
static AutoState autoState = AUTO_IDLE;

static unsigned long autoStartMs = 0;

static void autoEnter() {
  autoStartMs = millis();
  autoState = AUTO_STAGE1;
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
      TurnLevels_runTest();     // blocks; okay because it's explicitly a test state
      Drive_stopAll();
      autoState = AUTO_DONE;
      Serial.println("TurnLevels test complete.");
      return true;
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
