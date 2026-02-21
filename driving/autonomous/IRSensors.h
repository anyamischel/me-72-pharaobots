#pragma once
#include <Arduino.h>

void IRSensors_init();
void IRSensors_update();

// Expose read-only sensor states (0/1 from digitalRead)
extern int L;
extern int C;
extern int R;
