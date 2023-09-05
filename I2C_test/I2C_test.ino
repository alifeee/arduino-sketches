// testing I2C communication


#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(0xaa);        // send 0000 1010
  Wire.endTransmission();    // stop transmitting

  delay(1000); 
}
