#include "dune_Clockface.h"

namespace dune {

Clockface::Clockface(Adafruit_GFX* display) {
  _display = display;
  Locator::provide(display);
}

Clockface::~Clockface() {
  // Cleanup any dynamically allocated resources
}

void Clockface::setup(CWDateTime *dateTime) {
  _dateTime = dateTime;
  
  // Initialize display
  drawBackground();
  updateTime();
}

void Clockface::update() {
  // Update time display every minute
  if (millis() - lastMillisTime >= 60000) {
    updateTime();
    lastMillisTime = millis();
  }
  
  // Add animation logic here
  if (millis() - lastMillis >= 100) {
    // Animation frame update
    lastMillis = millis();
  }
}

void Clockface::updateTime() {
  // TODO: Implement time display
  // Example: Draw hour and minute
}

void Clockface::drawBackground() {
  // TODO: Implement Dune-themed background
  // Example: Desert sand, spice colors, etc.
  Locator::getDisplay()->fillRect(0, 0, 64, 64, 0x0000);
}

}  // namespace dune
