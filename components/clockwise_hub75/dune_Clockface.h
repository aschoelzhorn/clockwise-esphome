#pragma once

#include <Adafruit_GFX.h>
#include "Tile.h"
#include "Locator.h"
#include "Game.h"
#include "Object.h"
#include "ImageUtils.h"
// Commons
#include "IClockface.h"
#include "CWDateTime.h"

namespace dune {
class Clockface : public IClockface {
  private:
    Adafruit_GFX* _display;

    // Timing
    unsigned long lastMillis = 0;
    unsigned long lastMillisTime = 0;
    unsigned long lastBackgroundChange = 0;
    
    // Background cycling
    int currentBackgroundIndex = 0;
    static const int BACKGROUND_COUNT = 5;
    static const unsigned long BACKGROUND_CHANGE_INTERVAL = 5000; // 5 seconds
    
    void drawBackground() {
      // Simple color cycling for now to ensure linking works
      uint16_t colors[5] = {0xF800, 0x07E0, 0x001F, 0xFFE0, 0xF81F}; // Red, Green, Blue, Yellow, Magenta
      uint16_t currentColor = colors[currentBackgroundIndex];
      Locator::getDisplay()->fillRect(0, 0, 64, 64, currentColor);
    }
    
    void cycleBackground() {
      currentBackgroundIndex = (currentBackgroundIndex + 1) % BACKGROUND_COUNT;
      drawBackground();
    }

public:
    Clockface(Adafruit_GFX* display) : _display(display) {
      Locator::provide(display);
    }
    
    ~Clockface() = default;
    
    void setup(CWDateTime* dateTime) override {
      lastBackgroundChange = millis();
      drawBackground();
    }
    
    void update() override {
      // Update background every 5 seconds
      if (millis() - lastBackgroundChange >= BACKGROUND_CHANGE_INTERVAL) {
        cycleBackground();
        lastBackgroundChange = millis();
      }
    }
};
}  // namespace dune
