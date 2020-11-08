//functions from esp32-hal-ledc.h
//futaba S30003: pulse width: 500-3000 micro seconds
//1ms=0 degree | 2ms = 180degree while duty cycle is 20ms
//PWM freq of 5kHZ max of resolution of 13bits: 2^13, 0.012%
//8bit resolution: 4kHz~8kHz
//delay(ms)

//GPIO PIN for Servo Control Signal
const int servo_pin = 1; 

//// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
 
void setup(){
  ledcSetup(ledChannel, freq, resolution);
  
  ledcAttachPin(servo_pin, ledChannel);
}
 
void loop(){
  for(int pos = 0; pos <= 255; pos++){   
    ledcWrite(ledChannel, pos);
    delay(15);
  }
  for(int pos = 255; pos >= 0; pos--){
    ledcWrite(ledChannel, pos);   
    delay(15);
  }
}
