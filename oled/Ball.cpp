#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Ball.h"

Ball::Ball(int ballX, int ballY, float ballVel, int ballSize, Adafruit_SSD1306 *userDisplay) {
  x = ballX;
  y = ballY;
  pxSize = ballSize;
  velX = ballVel * 1.5;
  velY = 0;
  originalVel = ballVel;
  display = userDisplay;
}

void Ball::draw() {
  display->fillRect(x, y, pxSize, pxSize, SSD1306_WHITE);
}

void Ball::move() {
  x += (int) velX;
  y += (int) velY;
}
