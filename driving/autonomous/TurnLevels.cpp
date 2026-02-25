#include "TurnLevels.h"
#include "Drive.h"
#include <Arduino.h>

struct LR { int16_t l; int16_t r; };

static constexpr LR kTable[] = {
  {  40,   40},  // STRAIGHT
  { 45,   35},  // VEER_RIGHT
  { 45,   25},  // STEER_RIGHT
  { 50,   5},  // TURN_RIGHT
  { 50,    -15},  // HOOK_RIGHT
  { 40, -40},  // SPIN_RIGHT
  {  35,  45},  // VEER_LEFT
  {  25,  45},  // STEER_LEFT
  {  5,  50},  // TURN_LEFT
  {   -15,  50},  // HOOK_LEFT
  {-40,  40},  // SPIN_LEFT
};

void TurnLevels_apply(TurnLevel lvl) {
  // speed is unused in this version; remove it from the header too if you want
  const LR p = kTable[(uint8_t)lvl];
  Drive_setLeftRight((int)p.l, (int)p.r)
}
