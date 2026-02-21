#include "Config.h"
#include "Drive.h"
#include "IRSensors.h"
#include "Brain.h"

void setup() {
  Drive_init();
  IRSensors_init();
  Brain_init();
}

void loop() {
  Brain_update();
  delay(20);
}
