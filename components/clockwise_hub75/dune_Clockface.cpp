#include "dune_Clockface.h"
#include "dune_assets.h"

namespace dune {

// Array of pointers to background images
const uint16_t* backgroundImages[5] = {
  dune_background64x64,
  dune_sandworm64x64,
  dune_paul_sandworm64x64,
  dune_ornithopter64x64,
  dune_chani64x64
};

Clockface::Clockface(Adafruit_GFX* display) {
  _display = display;
  Locator::provide(display);
}

Clockface::~Clockface() {
  // Cleanup any dynamically allocated resources
}

void Clockface::setup(CWDateTime* dateTime) {
  
  // Initialize timing
  lastBackgroundChange = millis();
  
  // Initialize display
  drawBackground();
}

void Clockface::update() {
  // Update background every 5 seconds
  if (millis() - lastBackgroundChange >= BACKGROUND_CHANGE_INTERVAL) {
    cycleBackground();
    lastBackgroundChange = millis();
  }
  
  // Update time display every minute
  if (millis() - lastMillisTime >= 60000) {
    lastMillisTime = millis();
  }
  
  // Add animation logic here
  if (millis() - lastMillis >= 100) {
    // Animation frame update
    lastMillis = millis();
  }
}

void Clockface::drawBackground() {
  // Clear the display first
  Locator::getDisplay()->fillRect(0, 0, 64, 64, 0x0000);
  
  // Draw the current background image
  const uint16_t* currentBackground = backgroundImages[currentBackgroundIndex];
  
  // Draw the 64x64 RGB565 image
  for (int y = 0; y < 64; y++) {
    for (int x = 0; x < 64; x++) {
      uint16_t color = pgm_read_word(&currentBackground[y * 64 + x]);
      Locator::getDisplay()->drawPixel(x, y, color);
    }
  }
}

void Clockface::cycleBackground() {
  // Move to next background
  currentBackgroundIndex = (currentBackgroundIndex + 1) % BACKGROUND_COUNT;
  
  // Redraw background with new image
  drawBackground();
}

}  // namespace dune
