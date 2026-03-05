#include "Memory.h"

// --- Memory ---

Memory::Memory(float T, float threshold)
  : level(0.0f), state(false), T(T), threshold(threshold), lastTime(0) {}

void Memory::init() {
  lastTime = millis();
}

bool Memory::update(float raw) {
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0f;
  lastTime = now;
  level += (dt / T) * (raw - level);
  if (level > threshold)             state = true;
  else if (level < 1.0f - threshold) state = false;
  return state;
}

// --- MemoryLevel ---

MemoryLevel::MemoryLevel(float T)
  : level(0.0f), T(T), lastTime(0) {}

void MemoryLevel::init() {
  lastTime = millis();
}

float MemoryLevel::update(float raw) {
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0f;
  lastTime = now;
  level += (dt / T) * (raw - level);
  return level;
}