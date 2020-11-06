#include <ESP32Servo.h>

// create four servo objects 
Servo servo1;

// Published values for SG90 servos; adjust if needed
int minUs = 1000;
int maxUs = 2000;

// for the ESP32-S2 the GPIO pins are 1-21,26,33-42
int servo1Pin = 1;

int pos = 0;      // position in degrees
void setup() {
  Serial.begin(115200);
  servo1.setPeriodHertz(50);      // Standard 50hz servo
}

void loop() {
  servo1.attach(servo1Pin, minUs, maxUs);
  for (pos = 0; pos <= 180; pos += 1) { // sweep from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo1.write(pos);
    Serial.println(servo1.read());
    delay(1);             // waits 20ms for the servo to reach the position
  }
  Serial.println(getPwm());
  for (pos = 180; pos >= 0; pos -= 1) { // sweep from 180 degrees to 0 degrees
    servo1.write(pos);
    Serial.println(servo1.read());
    delay(1);
  }
  servo1.detach();
  delay(5000);

}
