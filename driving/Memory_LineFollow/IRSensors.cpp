#include "IRSensors.h"
#include "Config.h"

int L = 0;
int C = 0;
int R = 0;

void IRSensors_init() {
  pinMode(IR_LED_PIN, OUTPUT);
  pinMode(IR_PIN_L, INPUT_PULLUP);
  pinMode(IR_PIN_C, INPUT_PULLUP);
  pinMode(IR_PIN_R, INPUT_PULLUP);
}

void IRSensors_update() {
  L = digitalRead(IR_PIN_L);
  C = digitalRead(IR_PIN_C);
  R = digitalRead(IR_PIN_R);

  // Optional debug (comment out when not needed) cuz it creates a lot of lag
  Serial.print("L C R: ");
  Serial.print(L); Serial.print(" ");
  Serial.print(C); Serial.print(" ");
  Serial.println(R);
}
