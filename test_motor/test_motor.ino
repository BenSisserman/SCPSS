#include <ESP32Servo.h>

// create four servo objects 
Servo servo1;
// Published values for SG90 servos; adjust if needed
int minUs = 1000;
int maxUs = 2000;

// These are all GPIO pins on the ESP32
// Recommended pins include 2,4,12-19,21-23,25-27,32-33
// for the ESP32-S2 the GPIO pins are 1-21,26,33-42
int servo1Pin = 1;

int pos = 0;      // position in degrees
ESP32PWM pwm;
void setup() {
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  Serial.begin(115200);
  servo1.setPeriodHertz(50);      // Standard 50hz servo
}

void loop() {
  servo1.attach(servo1Pin, minUs, maxUs);
  pwm.attachPin(37, 10000);
  for (pos = 0; pos <= 180; pos += 1) { // sweep from 0 degrees to 180 degrees
    // in steps of 1 degree
    Serial.println(pos);
    servo1.write(pos);
    delay(1);             // waits 20ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // sweep from 180 degrees to 0 degrees
    Serial.println(pos);
    servo1.write(pos);
    delay(1);
  }
  servo1.detach();
  pwm.detachPin(27);
  delay(5000);
}
