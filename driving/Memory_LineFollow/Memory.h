#pragma once
#include <Arduino.h>

class Memory {
public:
  float level;
  bool state;
  float T;
  float threshold;
  unsigned long lastTime;

  Memory(float T = 0.1f, float threshold = 0.63f);
  void init();
  bool update(float raw);
};

class MemoryLevel {
public:
  float level;
  float T;
  unsigned long lastTime;

  MemoryLevel(float T = 0.2f);
  void init();
  float update(float raw);
};