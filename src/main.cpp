#define SERIAL_BUFFER_SIZE 1024

#include <Arduino.h>

#include "galvolib/Laser.h"

// Create laser instance (with laser pointer connected to digital pin 5)
Laser laser(5);
bool isOn = false;

void setup() {  
  laser.init();
  Serial.begin(115200);
  laser.setEnable3D(false);
  laser.off();
}

/*
int screen_y(int y, int h) {
    // we find we have to flip y
    // also, y 0 thru 95 and 928 thru 1023 are not used
    return (h as i32 - ((y - 96) as i32 * h as i32 / 832));
}

int screen_x(int x, int w) {
    return (x as i32 * w as i32 / 1024);
}*/

void draw(int x, int y, int z) {
  if(z > 0){
    if(!isOn) { // handle consecutive calls
        //laser.sendto(x,y);  //uncomment if random lines start showing up
        laser.pwm(z*16);
        isOn = true;
    }
  } else {
    if(isOn) {
        laser.off();
        isOn = false;
    }
  }
  laser.sendto(x,y); 
}

void loop() {
  while(Serial.available()) {
    uint8_t z = Serial.read();
    uint16_t x = Serial.read() | (Serial.read() << 8);
    uint16_t y = Serial.read() | (Serial.read() << 8);
    draw(x,y,z);
  }
}