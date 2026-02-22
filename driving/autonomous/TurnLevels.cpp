#include "TurnLevels.h"
#include "Drive.h"
#include <Arduino.h>

struct LR { int16_t l; int16_t r; };

static constexpr LR kTable[] = {
  {  90,   90},  // STRAIGHT
  { 100,   80},  // VEER_RIGHT
  { 100,   60},  // STEER_RIGHT
  { 110,   40},  // TURN_RIGHT
  { 110,    0},  // HOOK_RIGHT
  { 110, -110},  // SPIN_RIGHT
  {  80,  100},  // VEER_LEFT
  {  60,  100},  // STEER_LEFT
  {  40,  110},  // TURN_LEFT
  {   0,  110},  // HOOK_LEFT
  {-110,  110},  // SPIN_LEFT
};

void TurnLevels_apply(TurnLevel lvl, int /*speed*/) {
  // speed is unused in this version; remove it from the header too if you want
  const LR p = kTable[(uint8_t)lvl];
  Drive_setLeftRight((int)p.l, (int)p.r);
}
