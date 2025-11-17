/*
  L298N H-Bridge Demo
  l298-demo.ino

Built off of code from:  
  DroneBot Workshop 2022
  https://dronebotworkshop.com

  This code will turn on one of the motors at full speed indefinitely.
*/
 
// Motor Connections (ENA & ENB must use PWM pins)
#define IN1 9
#define IN2 8


#define ENA 10

 
void motorAccel() {
  for (int i = 0; i < 256; i++) {
    analogWrite(ENA, i);

    delay(20);
  }
}
 
void motorDecel() {
  for (int i = 255; i >= 0; --i) {
    analogWrite(ENA, i);

    delay(20);
  }
}
 
void setup() {
 
  // Set motor connections as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
 
 
  pinMode(ENA, OUTPUT);
 
 
  // Start with motors off
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

 
  analogWrite(ENA, 0);

}
 
void loop() {
 
  // Set motor forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  analogWrite(ENA, 255);
 
}