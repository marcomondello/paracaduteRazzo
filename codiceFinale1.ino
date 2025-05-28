#include <Arduino_BMI270_BMM150.h>
#include <Servo.h>

Servo mioServo;

// Variabili IMU
float ax, ay, az;
float accx = 0.0;
float v = 0.0;
float ti, tf, dt;

// Stato
bool servoAttivato = false;
bool superatoPicco = false;

void setup() {
  Serial.begin(115200);
 // while (!Serial);

  if (!IMU.begin()) {
    Serial.println("IMU non trovato!");
    while (1);
  }

  Serial.println("Inizio acquisizione...");

  mioServo.attach(A0); // Servo sul pin A0
  mioServo.write(0);   // Posizione iniziale

  ti = millis();
}

void loop() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);

    // Calcolo accelerazione netta lungo l'asse X (verticale)
    accx = (ax * 9.81) - 9.81;

    // Calcolo delta tempo
    tf = millis();
    dt = (tf - ti) / 1000.0;

    // Integrazione accelerazione per ottenere velocità
    v = v + accx * dt;

    Serial.print("accx: "); Serial.print(accx, 3);
    Serial.print("  vel: "); Serial.println(v, 3);

    // Se ho rilevato una spinta significativa, abilito rilevamento punto alto
    if (accx > 3.0) {
      superatoPicco = true;
    }

    // Attivo il servo se raggiunto il punto più alto (vel ≈ 0 e acc negativa)
    if (superatoPicco && !servoAttivato && abs(v) < 0.2 && accx < 0.0) {
      mioServo.write(90);  // Aziona il servo
      servoAttivato = true;
      Serial.println("==> Servo attivato al punto più alto!");
    }

    ti = tf;
    delay(20); // Piccolo ritardo per ridurre rumore
  }
}
