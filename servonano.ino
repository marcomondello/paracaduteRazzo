#include <Servo.h>
#define pinServo 12
Servo mioServo;


void setup() {
  mioServo.attach(pinServo);
  mioServo.write(0);               // Posizione iniziale
}

void loop() {
  mioServo.write(180);
  delay(700);
  mioServo.write(0);
  delay(700);
}
