#pragma once

#include <Adafruit_GFX.h>
#include "esphome/components/hub75/hub75_component.h"

// Adapter to make HUB75Display compatible with Adafruit_GFX-based code
class GFXWrapper : public Adafruit_GFX {
 public:
  enum class ColorChannelOrder : uint8_t {
    RGB = 0,
    RBG = 1,
    GRB = 2,
    GBR = 3,
    BRG = 4,
    BGR = 5
  };

  GFXWrapper(esphome::hub75::HUB75Display *display)
      : Adafruit_GFX(display->get_width(), display->get_height()), display_(display) {}

  void set_color_order(ColorChannelOrder order) { color_order_ = order; }
  ColorChannelOrder get_color_order() const { return color_order_; }

  void drawPixel(int16_t x, int16_t y, uint16_t color) override {
    // Convert RGB565 to RGB888
    // Note: Adafruit_GFX RGB565 is actually BGR order, so we swap R and B
    uint8_t b = ((color >> 11) & 0x1F) << 3;  // Blue from high bits
    uint8_t g = ((color >> 5) & 0x3F) << 2;   // Green from middle bits
    uint8_t r = (color & 0x1F) << 3;          // Red from low bits

    uint8_t out_r = r;
    uint8_t out_g = g;
    uint8_t out_b = b;
    apply_color_order_(r, g, b, out_r, out_g, out_b);

    display_->draw_pixel_at(x, y, esphome::Color(out_r, out_g, out_b));
  }

  void fillScreen(uint16_t color) {
    // Convert RGB565 to RGB888
    // Note: Adafruit_GFX RGB565 is actually BGR order, so we swap R and B
    uint8_t b = ((color >> 11) & 0x1F) << 3;  // Blue from high bits
    uint8_t g = ((color >> 5) & 0x3F) << 2;   // Green from middle bits
    uint8_t r = (color & 0x1F) << 3;          // Red from low bits

    uint8_t out_r = r;
    uint8_t out_g = g;
    uint8_t out_b = b;
    apply_color_order_(r, g, b, out_r, out_g, out_b);

    display_->fill(esphome::Color(out_r, out_g, out_b));
  }

 private:
  esphome::hub75::HUB75Display *display_;
  ColorChannelOrder color_order_{ColorChannelOrder::RGB};

  void apply_color_order_(uint8_t in_r, uint8_t in_g, uint8_t in_b,
                          uint8_t &out_r, uint8_t &out_g, uint8_t &out_b) const {
    switch (color_order_) {
      case ColorChannelOrder::RGB:
        out_r = in_r;
        out_g = in_g;
        out_b = in_b;
        break;
      case ColorChannelOrder::RBG:
        out_r = in_r;
        out_g = in_b;
        out_b = in_g;
        break;
      case ColorChannelOrder::GRB:
        out_r = in_g;
        out_g = in_r;
        out_b = in_b;
        break;
      case ColorChannelOrder::GBR:
        out_r = in_g;
        out_g = in_b;
        out_b = in_r;
        break;
      case ColorChannelOrder::BRG:
        out_r = in_b;
        out_g = in_r;
        out_b = in_g;
        break;
      case ColorChannelOrder::BGR:
        out_r = in_b;
        out_g = in_g;
        out_b = in_r;
        break;
    }
  }
};
