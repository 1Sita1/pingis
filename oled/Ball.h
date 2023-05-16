#ifndef Ball_h
#define Ball_h

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Arduino.h"

class Ball {
  public:
    Ball(int ballX, int ballY, float ballVel, int ballSize, Adafruit_SSD1306 *userDisplay);
    int x;
    int y;
    int pxSize;
    float velX;
    float velY;
    float originalVel;
    Adafruit_SSD1306 *display;
    void draw();
    void move();
};

#endif
