#include "Arduino.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Pad.h"

Pad::Pad(int padX, int padY, int padWidth, int padHeight, Adafruit_SSD1306 *userDisplay) {
  x = padX;
  y = padY;
  vel = 0;
  width = padWidth;
  height = padHeight;
  display = userDisplay;
}

void Pad::draw() {
  display->fillRect(x, y, width, height, SSD1306_WHITE);
}

// Updating physics
void Pad::move() {
  if (y + vel <= 0 || y + height + vel >= display->height()) {
    y = constrain(y, 0, display->height() - height);
    return;
  }
  y += (int) vel;
}
