

#include <ArduinoBLE.h>
#include "Arduino_BMI270_BMM150.h"
float x,y,z;

 
BLEService accService("180F");

// Bluetooth® Low Energy Battery Level Characteristic
BLECharacteristic accValChar("2A19", BLERead | BLENotify, 4); // 4 byte per float

int oldBatteryLevel = 0;  // last battery level reading from analog input
long previousMillis = 0;  // last time the battery level was checked, in ms

void setup() {
  Serial.begin(9600);    // initialize serial communication
  while (!Serial);

  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  /* Set a local name for the Bluetooth® Low Energy device
     This name will appear in advertising packets
     and can be used by remote devices to identify this Bluetooth® Low Energy device
     The name can be changed but maybe be truncated based on space left in advertisement packet
  */
  BLE.setLocalName("accMonitor");
  BLE.setAdvertisedService(accService); // add the service UUID
  accService.addCharacteristic(accValChar); // add the battery level characteristic
  BLE.addService(accService); // Add the battery service
  //accValChar.writeValue(0); // set initial value for this characteristic

  /* Start advertising Bluetooth® Low Energy.  It will start continuously transmitting Bluetooth® Low Energy
     advertising packets and will be visible to remote Bluetooth® Low Energy central devices
     until it receives a new connection */

  // start advertising
  BLE.advertise();

  Serial.println("Bluetooth® device active, waiting for connections...");
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void loop() {
  // wait for a Bluetooth® Low Energy central
  BLEDevice central = BLE.central();

  // if a central is connected to the peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

    // check the battery level every 200 ms
    // while the central is connected:
    while (central.connected()) {
      //leggi acc
      if (IMU.accelerationAvailable()) {
          IMU.readAcceleration(x, y, z);
          sendFloat(x);
          delay(200);
}
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}

void sendFloat(float val) {
  byte buffer[4];
  memcpy(buffer, &val, 4);
  accValChar.writeValue(buffer, 4);
}
