#pragma once

#include <Adafruit_GFX.h>
#include "esphome/components/hub75/hub75_component.h"

// Adapter to make HUB75Display compatible with Adafruit_GFX-based code
class GFXWrapper : public Adafruit_GFX {
 public:
  GFXWrapper(esphome::hub75_display::HUB75Display *display)
      : Adafruit_GFX(display->get_width(), display->get_height()), display_(display) {}

  void drawPixel(int16_t x, int16_t y, uint16_t color) override {
    // Convert RGB565 to RGB888
    // Note: Adafruit_GFX RGB565 is actually BGR order, so we swap R and B
    uint8_t b = ((color >> 11) & 0x1F) << 3;  // Blue from high bits
    uint8_t g = ((color >> 5) & 0x3F) << 2;   // Green from middle bits
    uint8_t r = (color & 0x1F) << 3;          // Red from low bits
    display_->draw_pixel_at(x, y, esphome::Color(r, g, b));
  }

  void fillScreen(uint16_t color) {
    // Convert RGB565 to RGB888
    // Note: Adafruit_GFX RGB565 is actually BGR order, so we swap R and B
    uint8_t b = ((color >> 11) & 0x1F) << 3;  // Blue from high bits
    uint8_t g = ((color >> 5) & 0x3F) << 2;   // Green from middle bits
    uint8_t r = (color & 0x1F) << 3;          // Red from low bits
    display_->fill(esphome::Color(r, g, b));
  }

 private:
  esphome::hub75_display::HUB75Display *display_;
};
