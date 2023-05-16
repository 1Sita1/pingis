#ifndef Pad_h
#define Pad_h

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Arduino.h"

class Pad {
  public:
    Pad(int padX, int padY, int padWidth, int padHeight, Adafruit_SSD1306 *userDisplay);
    int x;
    int y;
    int width;
    int height;
    float vel;
    Adafruit_SSD1306 *display;
    void draw();
    void move();
};

#endif
