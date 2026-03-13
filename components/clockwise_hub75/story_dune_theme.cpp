#include "dune_theme.h"
#include "dune_font.h"
#include "esphome/core/log.h"
#include <cmath>
#include <cstring>

static const char *const TAG = "DuneTheme";

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef constrain
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#endif

namespace dune {

// ==================== Act Initialization ====================

DuneTheme::DuneTheme() : frameCount_(0) {
  // Initialize 6 acts with their respective data
  // Each act contains: id, name, phrase pool, font color, background image
  
  acts_[0] = Act(0, "The Desert Sleeps",
                 PHRASES_TIME,
                 DIM_SAND,
                 dune_act1);

  acts_[1] = Act(1, "Spice Awakens",
                 PHRASES_DESERT,
                 SPICE_AMBER,
                 dune_act2);

  acts_[2] = Act(2, "The Watchers",
                 PHRASES_POWER,
                 HIGH_CONTRAST_WHITE,
                 dune_act3);

  acts_[3] = Act(3, "The Maker Stirs",
                 PHRASES_DANGER,
                 BRIGHT_SAND,
                 dune_act4);

  acts_[4] = Act(4, "Storm of Fate",
                 PHRASES_DANGER,
                 RED_DANGER,
                 dune_act5);

  acts_[5] = Act(5, "Silence & Survival",
                 PHRASES_SURVIVAL,
                 COOL_BROWN,
                 dune_act6);

  // Initialize ambient states
  ambientHeat_.enabled = false;
  ambientHeat_.lastUpdate = 0;
  ambientHeat_.phase = 0.0f;

  ambientShadow_.shadowX = 0;
  ambientShadow_.shadowY = 10;
  ambientShadow_.shadowDx = 1;
  ambientShadow_.lastUpdate = 0;

  ambientTremor_.lastUpdate = 0;
}

DuneTheme::~DuneTheme() {
  // Acts are stack-allocated, no cleanup needed
}

Act* DuneTheme::getAct(uint8_t actId) {
  if (actId < getActCount()) {
    return &acts_[actId];
  }
  return nullptr;
}

// ==================== Rendering ====================

// This implementation keeps rendering logic in the theme for maximum flexibility
// Each theme can customize how backgrounds, text, and effects are drawn

void DuneTheme::renderBackground(GFXWrapper* gfx, uint8_t actId) {
  if (actId >= 6) return;

  Act* act = getAct(actId);
  if (!act) return;

  const uint16_t* bg = act->getBackground();
  if (!bg) {
    ESP_LOGE(TAG, "renderBackground: bg is nullptr for act %d", actId);
    return;
  }

  // Draw 64x64 RGB565 bitmap
  gfx->drawRGBBitmap(0, 0, bg, 64, 64);
}

void DuneTheme::renderAmbientEffect(GFXWrapper* gfx, uint8_t actId, uint32_t frameCount) {
  if (actId >= 6) return;

  // Route to act-specific ambient effect renderer
  switch (actId) {
    case 0: renderAmbientEffect_I(gfx, frameCount); break;
    case 1: renderAmbientEffect_II(gfx, frameCount); break;
    case 2: renderAmbientEffect_III(gfx, frameCount); break;
    case 3: renderAmbientEffect_IV(gfx, frameCount); break;
    case 4: renderAmbientEffect_V(gfx, frameCount); break;
    case 5: renderAmbientEffect_VI(gfx, frameCount); break;
  }
}

void DuneTheme::renderTimeDisplay(GFXWrapper* gfx, uint8_t actId, CWDateTime& dt) {
  if (actId >= 6) return;

  Act* act = getAct(actId);
  if (!act) return;

  gfx->setTextSize(1);
  gfx->setTextColor(act->getFontColor());

  // Placement
  int x = 10;  // x_start
  int y = 34;  // y_start

  gfx->setCursor(x, y);

  char buf[6];
  uint32_t now = millis();
  bool showColon = (now / 1000) % 2;  // blink every second

  snprintf(buf, sizeof(buf),
           showColon ? "%02d:%02d" : "%02d %02d",
           dt.getHour(), dt.getMinute());

  gfx->print(buf);
}

void DuneTheme::renderTextField(GFXWrapper* gfx, uint8_t actId, const char* text) {
  if (actId >= 6 || !text) return;

  Act* act = getAct(actId);
  if (!act) return;

  uint16_t color = act->getFontColor();
  
  // Set new text if needed
  if (textState_.phrase != text) {
    textState_.phrase = text;
    textState_.phase = TEXT_FADE_IN;
    textState_.phaseStart = millis();
  }
  
  // Update state machine
  updateTextState(millis());
  
  // Calculate alpha based on current phase
  uint8_t alpha = 255;
  uint32_t elapsed = millis() - textState_.phaseStart;
  
  switch (textState_.phase) {
    case TEXT_FADE_IN:
      alpha = (elapsed * 255) / TEXT_FADE_MS;
      if (alpha > 255) alpha = 255;
      break;
      
    case TEXT_HOLD:
      alpha = 255;
      break;
      
    case TEXT_FADE_OUT:
      alpha = 255 - ((elapsed * 255) / TEXT_FADE_MS);
      if (alpha > 255) alpha = 0;  // Handle underflow
      break;
      
    case TEXT_QUIET:
    case TEXT_IDLE:
      return;  // Don't render during quiet/idle
  }
  
  drawPhraseBlended(gfx, text, color, alpha);
}

// ==================== Ambient Effect Implementations ====================

void DuneTheme::renderAmbientEffect_I(GFXWrapper* gfx, uint32_t frameCount) {
  // ACT I: The Desert Sleeps - Heat shimmer
  // Minimal motion, very subtle brightness modulation
  
  if (!ambientHeat_.enabled) return;

  uint32_t now = millis();
  // Cold desert = slow update
  if (now - ambientHeat_.lastUpdate < 120) return;
  ambientHeat_.lastUpdate = now;

  ambientHeat_.phase += 0.08f;
  if (ambientHeat_.phase > 6.28f)
    ambientHeat_.phase -= 6.28f;

  // Note: This implementation requires direct pixel manipulation
  // which may not be fully compatible with GFXWrapper
  // Placeholder for now
}

void DuneTheme::renderAmbientEffect_II(GFXWrapper* gfx, uint32_t frameCount) {
  // ACT II: Spice Awakens - Floating spice motes
  // Placeholder
}

void DuneTheme::renderAmbientEffect_III(GFXWrapper* gfx, uint32_t frameCount) {
  // ACT III: The Watchers - Shadow drift
  uint32_t now = millis();
  if (now - ambientShadow_.lastUpdate < 80) return;
  ambientShadow_.lastUpdate = now;

  ambientShadow_.shadowX += ambientShadow_.shadowDx;

  if (ambientShadow_.shadowX > 48 || ambientShadow_.shadowX < 4) {
    ambientShadow_.shadowDx = -ambientShadow_.shadowDx;
    ambientShadow_.shadowY = 8 + (ambientShadow_.shadowX % 16);
  }

  // Get current act's background
  Act* act = getAct(2);  // ACT III
  if (act) {
    drawShadowBand(gfx, ambientShadow_.shadowX, ambientShadow_.shadowY, act->getBackground());
  }
}

void DuneTheme::renderAmbientEffect_IV(GFXWrapper* gfx, uint32_t frameCount) {
  // ACT IV: The Maker Stirs - Subsurface tremor
  uint32_t now = millis();
  if (now - ambientTremor_.lastUpdate < AmbientTremorState::UPDATE_MS) return;
  ambientTremor_.lastUpdate = now;

  Act* act = getAct(3);  // ACT IV
  if (!act) return;

  const uint16_t* bg = act->getBackground();

  // Draw 8–12 small ripples per frame
  for (uint8_t i = 0; i < 10; i++) {
    // Random-ish positions using now for deterministic pseudo-random
    uint8_t y = 32 + ((i * 7 + now / 150) % 20);  // avoid top-center (0–31)
    uint8_t x = (i * 5 + (now / 100)) % 48;       // ripple within safe width

    drawTremorRipple(gfx, x, y, bg);
  }
}

void DuneTheme::renderAmbientEffect_V(GFXWrapper* gfx, uint32_t frameCount) {
  // ACT V: Storm of Fate - Wind-driven sand
  // Placeholder
}

void DuneTheme::renderAmbientEffect_VI(GFXWrapper* gfx, uint32_t frameCount) {
  // ACT VI: Silence & Survival - Dust fall
  // Placeholder
}

// ==================== Helper Methods ====================

int DuneTheme::textWidth(const char* s) const {
  if (!s) return 0;
  return strlen(s) * (FONT_W + CHAR_SPACING) - CHAR_SPACING;
}

void DuneTheme::drawCharBlended(GFXWrapper* gfx, char c, int x, int y, uint16_t color, uint8_t alpha) {
  uint8_t index = duneFontIndex(c);
  const uint8_t* glyph = dune_font5x7[index];

  for (int col = 0; col < FONT_W; col++) {
    uint8_t bits = pgm_read_byte(&glyph[col]);

    for (int row = 0; row < FONT_H; row++) {
      if (bits & (1 << row)) {
        int px = x + col;
        int py = y + row;

        if (px >= 0 && px < 64 && py >= 0 && py < 64) {
          // Note: Full blending requires reading pixels which GFXWrapper doesn't support
          // For now, just draw the pixel
          gfx->drawPixel(px, py, color);
        }
      }
    }
  }
}

void DuneTheme::drawPhraseBlended(GFXWrapper* gfx, const char* phrase, uint16_t color, uint8_t alpha) {
  if (!phrase) return;

  int w = textWidth(phrase);
  int x = (64 - w) / 2;
  int y = TEXT_Y;

  for (size_t i = 0; i < strlen(phrase); i++) {
    drawCharBlended(gfx, phrase[i], x, y, color, alpha);
    x += FONT_W + CHAR_SPACING;
 }
}

void DuneTheme::renderTextPhrase(GFXWrapper* gfx, const char* phrase, uint16_t color) {
  drawPhraseBlended(gfx, phrase, color, 255);
}

void DuneTheme::updateTextState(uint32_t frameTime) {
  // Update text fade in/hold/fade out state machine
  if (!textState_.phrase) {
    textState_.phase = TEXT_IDLE;
    return;
  }

  uint32_t elapsed = frameTime - textState_.phaseStart;

  switch (textState_.phase) {
    case TEXT_IDLE:
      // Waiting to start new text
      textState_.phaseStart = frameTime;
      textState_.phase = TEXT_FADE_IN;
      break;

    case TEXT_FADE_IN:
      if (elapsed >= TEXT_FADE_MS) {
        textState_.phase = TEXT_HOLD;
        textState_.phaseStart = frameTime;
      }
      break;

    case TEXT_HOLD:
      if (elapsed >= TEXT_HOLD_MS) {
        textState_.phase = TEXT_FADE_OUT;
        textState_.phaseStart = frameTime;
      }
      break;

    case TEXT_FADE_OUT:
      if (elapsed >= TEXT_FADE_MS) {
        textState_.phase = TEXT_QUIET;
        textState_.phaseStart = frameTime;
        textState_.phrase = nullptr;  // Clear phrase
      }
      break;

    case TEXT_QUIET:
      if (elapsed >= TEXT_QUIET_MS) {
        textState_.phase = TEXT_IDLE;
      }
      break;
  }
}

void DuneTheme::drawShadowBand(GFXWrapper* gfx, uint8_t xStart, uint8_t yStart, const uint16_t* bg) {
  const uint8_t width = 16;
  const uint8_t height = 6;
  
  if (!bg) return;

  for (uint8_t y = 0; y < height; y++) {
    for (uint8_t x = 0; x < width; x++) {
      uint8_t px = xStart + x;
      uint8_t py = yStart + y;
      if (px >= 64 || py >= 64) continue;

      uint16_t base = pgm_read_word(&bg[py * 64 + px]);
      uint16_t shaded = darken(base);

      gfx->drawPixel(px, py, shaded);
    }
  }
}

void DuneTheme::drawTremorRipple(GFXWrapper* gfx, uint8_t xStart, uint8_t yStart, const uint16_t* bg) {
  const uint8_t width = 8;
  const uint8_t height = 2;

  if (!bg) return;

  for (uint8_t y = 0; y < height; y++) {
    for (uint8_t x = 0; x < width; x++) {
      uint8_t px = xStart + x;
      uint8_t py = yStart + y;
      if (px >= 64 || py >= 64) continue;

      // Sample background
      uint16_t base = pgm_read_word(&bg[py * 64 + px]);
      // Darken for ripple
      uint16_t ripple = darken(base, 0.8f);  // darken 20%

      gfx->drawPixel(px, py, ripple);
    }
  }
}

// ==================== Color Utilities ====================

uint16_t DuneTheme::blend565(uint16_t bg, uint16_t fg, uint8_t alpha) {
  if (alpha < 16) alpha = 16;

  // alpha: 0 = bg, 255 = fg
  uint8_t br = r5(bg);
  uint8_t bgc = g6(bg);
  uint8_t bb = b5(bg);

  uint8_t fr = r5(fg);
  uint8_t fg_c = g6(fg);
  uint8_t fb = b5(fg);

  uint8_t r = (br * (255 - alpha) + fr * alpha) / 255;
  uint8_t g = (bgc * (255 - alpha) + fg_c * alpha) / 255;
  uint8_t b = (bb * (255 - alpha) + fb * alpha) / 255;

  return rgb565(r, g, b);
}

uint16_t DuneTheme::darken(uint16_t color, float factor) {
  uint8_t r = r5(color);
  uint8_t g = g6(color);
  uint8_t b = b5(color);

  r = (uint8_t)(r * factor);
  g = (uint8_t)(g * factor);
  b = (uint8_t)(b * factor);

  return rgb565(r, g, b);
}

}  // namespace dune
