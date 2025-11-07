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

// Array of pointers to background images - using unique images only to avoid duplicates
static const uint16_t* backgroundImages[7] = {
  dune_baron64x64,
  dune_ornithopter64x64,
  dune_chani64x64,
  dune_sandworm64x64,
  dune_paul_sandworm64x64,
  dune_baron_desert64x64,
  dune_baron_chamber64x64
};

class Clockface : public IClockface {
  private:
    Adafruit_GFX* _display;
    CWDateTime* _dateTime;  // Store the datetime reference

    // Timing
    unsigned long lastMillis = 0;
    unsigned long lastMillisTime = 0;
    unsigned long lastBackgroundChange = 0;
    
    // Background cycling
    int currentBackgroundIndex = 0;
    static const int BACKGROUND_COUNT = 7;  // Updated for 7 unique images
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
    
    void drawTime() {
      if (!_dateTime) return;
      
      // Get current time using CWDateTime methods
      int hour = _dateTime->getHour();
      int minute = _dateTime->getMinute();
      char timeStr[6];  // HH:MM format
      snprintf(timeStr, sizeof(timeStr), "%02d:%02d", hour, minute);
      
      // Set text properties for visibility on any background - smaller text
      Locator::getDisplay()->setTextColor(0xFFFF, 0x0000); // White text with black background
      Locator::getDisplay()->setTextSize(1);
      
      // Calculate center position for smaller text (each character is 5 pixels wide, 5 characters = 25 pixels at size 1)
      int textWidth = 25; // 5 chars * 5 pixels per char (6x8 font is 5 pixels wide)
      int x = (64 - textWidth) / 2;
      int y = 1; // Top of screen with smaller margin
      
      // Draw smaller black background rectangle for text readability
      Locator::getDisplay()->fillRect(x - 1, y - 1, textWidth + 2, 6, 0x0000);
      
      // Draw the time text
      Locator::getDisplay()->setCursor(x, y);
      Locator::getDisplay()->print(timeStr);
    }
    
    void cycleBackground() {
      currentBackgroundIndex = (currentBackgroundIndex + 1) % BACKGROUND_COUNT;
      drawBackground();
      drawTime(); // Redraw time on new background
    }

public:
    Clockface(Adafruit_GFX* display) : _display(display) {
      Locator::provide(display);
    }
    
    ~Clockface() = default;
    
    void setup(CWDateTime* dateTime) override {
      _dateTime = dateTime; // Store the datetime reference
      lastBackgroundChange = millis();
      drawBackground();
      drawTime(); // Draw initial time
    }
    
    void update() override {
      // Update background every 5 seconds
      if (millis() - lastBackgroundChange >= BACKGROUND_CHANGE_INTERVAL) {
        cycleBackground();
        lastBackgroundChange = millis();
      }
      
      // Update time display every minute (60 seconds)
      if (millis() - lastMillisTime >= 60000) {
        drawTime();
        lastMillisTime = millis();
      }
    }
};
}  // namespace dune
