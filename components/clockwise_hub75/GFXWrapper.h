#pragma once

#include <Adafruit_GFX.h>
#include "esphome/components/hub75/hub75_component.h"

// Adapter to make HUB75Display compatible with Adafruit_GFX-based code
class GFXWrapper : public Adafruit_GFX {
 public:
  GFXWrapper(esphome::hub75::HUB75Display *display)
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


  uint16_t darken(uint16_t color) {
      uint8_t r = (color >> 11) & 0x1F;
      uint8_t g = (color >> 5) & 0x3F;
      uint8_t b = color & 0x1F;

      r = (r * 3) / 4;
      g = (g * 3) / 4;
      b = (b * 3) / 4;

      return (r << 11) | (g << 5) | b;
  }

  uint16_t darken(uint16_t color, float factor) {
      uint8_t r = (color >> 11) & 0x1F;
      uint8_t g = (color >> 5) & 0x3F;
      uint8_t b = color & 0x1F;

      r = (uint8_t)(r * factor);
      g = (uint8_t)(g * factor);
      b = (uint8_t)(b * factor);

      return (r << 11) | (g << 5) | b;
  }

/*
  void drawCharBlended(char c, int x, int y, uint16_t color, uint8_t alpha) {
    uint8_t index = fontIndex(c);
    const uint8_t* glyph = dune_font5x7[index];

    for (int col = 0; col < FONT_W; col++) {
      uint8_t bits = pgm_read_byte(&glyph[col]);

      for (int row = 0; row < FONT_H; row++) {
        if (bits & (1 << row)) {
          int px = x + col;
          int py = y + row;

          uint16_t bg = fbGet(px, py);
          uint16_t blended = blend565(bg, color, alpha);
          fbSet(px, py, blended);
        }
      }
    }
  }



  void drawPhraseBlended(const char* phrase, uint16_t textColor, uint8_t alpha) {
      if (!phrase) return;

      int w = textWidth(phrase);
      int x = (64 - w) / 2;
      int y = TEXT_Y;

      for (size_t i = 0; i < strlen(phrase); i++) {
          drawCharBlended(phrase[i], x, y, textColor, alpha);
          x += FONT_W + CHAR_SPACING;
    }
  }

  uint16_t blend565(uint16_t bg, uint16_t fg, uint8_t alpha) {

    if (alpha < 16) alpha = 16;

    // alpha: 0 = bg, 255 = fg
    uint8_t br = (bg >> 11) & 0x1F;
    uint8_t bgc = (bg >> 5) & 0x3F;
    uint8_t bb = bg & 0x1F;

    uint8_t fr = (fg >> 11) & 0x1F;
    uint8_t fg_c = (fg >> 5) & 0x3F;
    uint8_t fb = fg & 0x1F;

    
    uint8_t r = (br * (255 - alpha) + fr * alpha) / 255;
    uint8_t g = (bgc * (255 - alpha) + fg_c * alpha) / 255;
    uint8_t b = (bb * (255 - alpha) + fb * alpha) / 255;

    return (r << 11) | (g << 5) | b;
}

int textWidth(const char* s) {
  return strlen(s) * (FONT_W + CHAR_SPACING) - CHAR_SPACING;
}
*/

 private:
  esphome::hub75::HUB75Display *display_;

  inline uint8_t r5(uint16_t c) { return (c >> 11) & 0x1F; }
  inline uint8_t g6(uint16_t c) { return (c >> 5) & 0x3F; }
  inline uint8_t b5(uint16_t c) { return c & 0x1F; }

  inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 11) | (g << 5) | b;
  }

};
