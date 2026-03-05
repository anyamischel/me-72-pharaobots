#pragma once
#include <Arduino.h>

void Drive_init();
void Drive_updateIBus();          // call each loop
bool Drive_ibusFrameOK();         // safety: returns false if no valid frame

// Manual driving based on RC receiver (implements your rcCH8 modes)
void Drive_manualUpdate();

// Low-level outputs (used by Brain autonomous)
void Drive_setLeftRight(int leftCmd, int rightCmd);  // cmds in [-127,127]
void Drive_stopAll();

// Intake control (Brain can call too, if needed)
void Drive_setIntakeMicroseconds(int us);

// Read switches/channels for Brain decisions
bool Drive_readAutoSwitch();
