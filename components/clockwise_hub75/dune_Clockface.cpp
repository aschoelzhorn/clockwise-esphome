#include "dune_Clockface.h"

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

void Clockface::setup(CWDateTime *dateTime) {
  _dateTime = dateTime;
  
  // Initialize timing
  lastBackgroundChange = millis();
  
  // Initialize display
  drawBackground();
  //updateTime();
}

void Clockface::update() {
  // Update background every 5 seconds
  if (millis() - lastBackgroundChange >= BACKGROUND_CHANGE_INTERVAL) {
    cycleBackground();
    lastBackgroundChange = millis();
  }
  
  // Update time display every minute
  if (millis() - lastMillisTime >= 60000) {
    //updateTime();
    lastMillisTime = millis();
  }
  
  // Add animation logic here
  if (millis() - lastMillis >= 100) {
    // Animation frame update
    lastMillis = millis();
  }
}

void Clockface::updateTime() {
  if (_dateTime == nullptr) return;
  
  // Get current time
  int hour = _dateTime->getHour();
  int minute = _dateTime->getMinute();
  
  // Convert to 12-hour format
  int displayHour = hour;
  if (displayHour == 0) displayHour = 12;
  else if (displayHour > 12) displayHour -= 12;
  
  // Clear time area (bottom part of display)
  Locator::getDisplay()->fillRect(0, 50, 64, 14, 0x0000);
  
  // Set text properties
  Locator::getDisplay()->setTextSize(1);
  Locator::getDisplay()->setTextColor(0xFFFF); // White text
  
  // Format time string
  char timeStr[6];
  sprintf(timeStr, "%02d:%02d", displayHour, minute);
  
  // Center the text (approximate)
  int textWidth = strlen(timeStr) * 6; // Approximate width
  int x = (64 - textWidth) / 2;
  
  // Draw time
  #Locator::getDisplay()->setCursor(x, 54);
  #Locator::getDisplay()->print(timeStr);
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
  
  // Redraw time on top of new background
  //updateTime();
}

}  // namespace dune
