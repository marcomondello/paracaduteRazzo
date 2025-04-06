#include <Servo.h>

Servo mioServo;

const int pinInput = 2;     // Pin che legge il segnale di attivazione
const int pinServo = 9;     // Pin collegato al servo

void setup() {
  //pinMode(pinInput, INPUT_PULLUP); // Usa resistenza interna pull-up
  mioServo.attach(pinServo);
  mioServo.write(0);               // Posizione iniziale
}

void loop() {
  bool attivazione = AnalogRead(pinInput);

  if (stato == HIGH) {
    mioServo.write(45);   // Vai a 45 gradi
  }

  //delay(20); // Piccola attesa per stabilità
}
