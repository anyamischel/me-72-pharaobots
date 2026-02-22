#pragma once
#include <stdint.h>

enum class TurnLevel : uint8_t {
  STRAIGHT,
  VEER_RIGHT,
  STEER_RIGHT,
  TURN_RIGHT,
  HOOK_RIGHT,
  SPIN_RIGHT,
  VEER_LEFT,
  STEER_LEFT,
  TURN_LEFT,
  HOOK_LEFT,
  SPIN_LEFT
};

// Apply a preset using Drive_setLeftRight. speed is 0..127.
void TurnLevels_apply(TurnLevel lvl, int speed);
