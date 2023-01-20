#define SERIAL_BUFFER_SIZE 1024

#include <Arduino.h>

#include <Wire.h>

#ifndef LED_BUILTIN
  #define LED_BUILTIN PC13
#endif

#include "galvolib/Laser.h"

#define BIG_MOVE_THRESH 10

// Create laser instance (with laser pointer connected to digital pin 5)
Laser laser(PB8); //, &WIRE);
//bool isOn = false;

uint16_t oldx, oldy;
uint8_t oldz;

void scani2c() {
  Serial.println("\nI2C Scan:");
  byte error, address;
  int nDevices;

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

void setup() {  
  laser.init();
  laser.setScale(1.);
  laser.setOffset(0,0);
  Serial.begin(921600);
  while(!Serial);
  laser.setEnable3D(false);
  laser.off();
  Wire.begin();

  scani2c();
  pinMode(LED_BUILTIN, OUTPUT);
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

void loop() {
  while(Serial.available()) {
    pinMode(LED_BUILTIN, LOW);
    uint8_t z = Serial.read(); // MSB FIRST
    uint16_t x = ((Serial.read() << 8) | Serial.read())*4;
    uint16_t y = ((Serial.read() << 8) | Serial.read())*4;
    //Serial.print("Got: ");
    //Serial.print(z);
    //Serial.print(',');
    //Serial.print(x);
    //Serial.print(',');
    //Serial.println(y);

    /*if(x > 4095) {
      x = 4095;
    }

    if(y > 4095) {
      y = 4095;
    }*/

    if(z == 0) {
    //  if(oldz > 0) {
        laser.pwm(0);
        laser.sendToDAC(x, y); // skip interpolation
        //laser.off();
    //  }
    } else if (z < 17) { // don't execute bad data
      laser.pwm(z*17);
      //int16_t dx = abs(x - oldx);
      //int16_t dy = abs(y - oldy);

      //if (dx + dy < BIG_MOVE_THRESH) { // filter travel moves
      //  laser.sendtoRaw(x,y);
      //} else {
        // dont interpolate big moves 
        laser.sendToDAC(x,y);
      //}
      //laser.on();
    }

    //oldx = x;
    //oldy = y;
    oldz = z;
    
    //laser.sendToDAC(x,y); // bypass fancy stuff
  }
  pinMode(LED_BUILTIN, HIGH);
}

