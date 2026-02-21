int sensorL = 2;
int sensorC = 3;
int sensorR = 4;
int ledpin = 13;


void setup() {
 pinMode(ledpin,OUTPUT);
 pinMode(sensorL, INPUT_PULLUP);
 pinMode(sensorC, INPUT_PULLUP);
 pinMode(sensorR, INPUT_PULLUP);
 Serial.begin(9600);
}




void loop(){
 int L = digitalRead(sensorL);
 int C = digitalRead(sensorC);
 int R = digitalRead(sensorR);
  Serial.print("L C R: ");
 Serial.print(L);
 Serial.print(" ");
 Serial.print(C);
 Serial.print(" ");
 Serial.println(R);
 // 0 0 0 is white
 // 1 1 1 is black
 // L C R
}
