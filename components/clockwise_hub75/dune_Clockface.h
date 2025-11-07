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

#include "dune_assets.h"

namespace dune {

// Array of pointers to background images
static const uint16_t* backgroundImages[5] = {
  dune_background64x64,
  dune_sandworm64x64,
  dune_paul_sandworm64x64,
  dune_ornithopter64x64,
  dune_chani64x64
};

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
      // Clear the display first
      Locator::getDisplay()->fillRect(0, 0, 64, 64, 0x0000);
      
      // Draw the current Dune background image
      const uint16_t* currentBackground = backgroundImages[currentBackgroundIndex];
      
      // Draw the 64x64 RGB565 image pixel by pixel
      for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
          uint16_t color = pgm_read_word(&currentBackground[y * 64 + x]);
          Locator::getDisplay()->drawPixel(x, y, color);
        }
      }
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
