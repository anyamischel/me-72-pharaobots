#include "TurnLevels.h"
#include "Drive.h"
#include <Arduino.h>

void TurnLevels_runTest() {

  const TurnLevel levels[] = {
    TurnLevel::STRAIGHT,
    TurnLevel::VEER_RIGHT,
    TurnLevel::STEER_RIGHT,
    TurnLevel::TURN_RIGHT,
    TurnLevel::HOOK_RIGHT,
    TurnLevel::SPIN_RIGHT,
    TurnLevel::VEER_LEFT,
    TurnLevel::STEER_LEFT,
    TurnLevel::TURN_LEFT,
    TurnLevel::HOOK_LEFT,
    TurnLevel::SPIN_LEFT
  };

  const int speed = 90;       // start conservative
  const int holdMs = 1500;    // how long each motion runs
  const int pauseMs = 800;    // stop between motions

  for (auto lvl : levels) {
    TurnLevels_apply(lvl, speed);
    delay(holdMs);
    Drive_stopAll();
    delay(pauseMs);
  }

  Drive_stopAll();
}
