#include "story_Clockface.h"
#include "story_act.h"
#include "story_font.h"
#include "esphome/core/log.h"
#include <Adafruit_GFX.h>
#include <pgmspace.h>

// Enable test mode to cycle acts every 15 seconds
#define TEST_ACT_CYCLING

namespace dune {

class FB_GFX : public Adafruit_GFX {
public:
    FB_GFX() : Adafruit_GFX(64, 64) {}

    void drawPixel(int16_t x, int16_t y, uint16_t color) override {
        if (x < 0 || y < 0 || x >= 64 || y >= 64) return;
        StoryClockface::framebuffer[y * 64 + x] = color;
    }
};

static FB_GFX fbGfx;

uint16_t StoryClockface::framebuffer[64 * 64];


static const char *const TAG = "StoryClockface";

StoryClockface::StoryClockface(IStoryTheme* theme, GFXWrapper* gfx) {
  if (!theme) {
    ESP_LOGE(TAG, "Theme cannot be null");
  }
  _theme = theme;
  _display = gfx;
  _currentActId = 0;
  _frameCount = 0;
  _currentPhrase = nullptr;
  _dateTime = nullptr;
  _now = 0;

  // TODO: not sure if Locator is needed, but we had this also in the dune Clockface constructor
  //Locator::provide(_display);
  
  // Initialize pointers to nullptr - objects will be created in setup()
  // _eventBus = nullptr;
  _dateTime = nullptr;
}

StoryClockface::~StoryClockface() {
  // Don't delete theme_ - caller owns it
    // delete _eventBus;
}

void StoryClockface::setup(CWDateTime* dt) {
  _dateTime = dt;
  if (!_dateTime) {
    ESP_LOGE(TAG, "CWDateTime pointer cannot be null");
    return;
  }
  
    // Create objects here instead of in constructor to avoid initialization order issues
    //_eventBus = new EventBus();
    // Provide EventBus after creation
    //Locator::provide(_eventBus);

  // Initialize with current act
  _currentActId = _theme->getCurrentActId(*_dateTime);
  Act* act = _theme->getAct(_currentActId);
  if (act) {
    _currentPhrase = act->getNewPhrase();
    ESP_LOGI(TAG, "StoryClockface initialized - Theme: %s, Act: %s",
             _theme->getThemeName(), act->getName());
  }
}

void StoryClockface::flushFramebuffer() {
    _display->drawRGBBitmap(0, 0, framebuffer, 64, 64);
}


void StoryClockface::update() {
  if (_dateTime && _display) {
    render(_display, *_dateTime);
  } else {
    if (!_dateTime) {
      ESP_LOGE(TAG, "update() failed: _dateTime is nullptr");
    }
    if (!_display) {
      ESP_LOGE(TAG, "update() failed: _display is nullptr");
    }
  }

}

void StoryClockface::render(GFXWrapper* gfx, CWDateTime& dt) {
  if (!_theme || !gfx) {
    return;
  }

  _now = millis();
  
  updateAct(dt);
  
  // Update phrase occasionally
  if (_frameCount % PHRASE_UPDATE_FRAMES == 0) {
    updatePhrase();
  }

  renderFrame(gfx, dt);
  _frameCount++;
}

void StoryClockface::updateAct(CWDateTime& dt) {
  uint8_t newActId;
  
#ifdef TEST_ACT_CYCLING
  // Test mode: Change act every 15 seconds
  static constexpr uint32_t TEST_ACT_MS = 15 * 1000;
  newActId = (_now / TEST_ACT_MS) % _theme->getActCount();
#else
  // Normal mode: Use theme's time-based act calculation
  newActId = _theme->getCurrentActId(dt);
#endif
  
  if (newActId != _currentActId) {
    // Start background transition
    Act* oldAct = _theme->getAct(_currentActId);
    Act* newAct = _theme->getAct(newActId);
    
    if (oldAct && newAct) {
      ESP_LOGI(TAG, "Act changed: %s", newAct->getName());
      
      _bgTransition.active = true;
      _bgTransition.from = oldAct->getBackground();
      _bgTransition.to = newAct->getBackground();
      _bgTransition.start = _now;
      
      _currentActId = newActId;
      _currentPhrase = newAct->getNewPhrase();
    }
  }
}

void StoryClockface::updatePhrase() {
  Act* act = _theme->getAct(_currentActId);
  if (act) {
    _currentPhrase = act->getNewPhrase();
  }
}

void StoryClockface::renderFrame(GFXWrapper* gfx, CWDateTime& dt) {
  // Layer 0: Clear
  fbClear(0x0000);  // Black

  // Layer 1: Background with crossfading
  renderBackgroundLayer();

  // Layer 2: Ambient effects (theme-specific)
  // _theme->renderAmbientEffect(gfx, _currentActId, _frameCount);

  // Layer 3: Major events (placeholder for future)
  // _theme->renderMajorEvents(gfx, _currentActId, _frameCount);

  // Layer 4: Time display
  renderTimeLayer();

  // Layer 5: Text overlay (phrases)
  renderTextLayer();
  
  // Flush framebuffer to display
  flushFramebuffer();
}

// ==================== Framebuffer Operations ====================

inline void StoryClockface::fbClear(uint16_t color) {
  for (uint16_t i = 0; i < 64 * 64; i++) {
    framebuffer[i] = color;
  }
}

inline uint16_t StoryClockface::fbGet(uint8_t x, uint8_t y) {
  return framebuffer[y * 64 + x];
}

inline void StoryClockface::fbSet(uint8_t x, uint8_t y, uint16_t color) {
  framebuffer[y * 64 + x] = color;
}

uint16_t StoryClockface::blend565(uint16_t bg, uint16_t fg, uint8_t alpha) {
  // Extract RGB components (5-6-5 format)
  uint8_t r1 = (bg >> 11) & 0x1F;
  uint8_t g1 = (bg >> 5) & 0x3F;
  uint8_t b1 = bg & 0x1F;
  
  uint8_t r2 = (fg >> 11) & 0x1F;
  uint8_t g2 = (fg >> 5) & 0x3F;
  uint8_t b2 = fg & 0x1F;
  
  // Blend
  uint8_t r = ((r1 * (255 - alpha)) + (r2 * alpha)) / 255;
  uint8_t g = ((g1 * (255 - alpha)) + (g2 * alpha)) / 255;
  uint8_t b = ((b1 * (255 - alpha)) + (b2 * alpha)) / 255;
  
  return (r << 11) | (g << 5) | b;
}

void StoryClockface::renderBackgroundLayer() {
  Act* currentAct = _theme->getAct(_currentActId);
  if (!currentAct) return;
  
  const uint16_t* currentBg = currentAct->getBackground();
  if (!currentBg) return;
  
  // No transition - just copy background to framebuffer
  if (!_bgTransition.active) {
    for (uint16_t i = 0; i < 64 * 64; i++) {
      framebuffer[i] = pgm_read_word(&currentBg[i]);
    }
    return;
  }
  
  // Transition active - blend between backgrounds
  uint32_t elapsed = _now - _bgTransition.start;
  
  if (elapsed >= _bgTransition.duration) {
    // End transition
    _bgTransition.active = false;
    for (uint16_t i = 0; i < 64 * 64; i++) {
      framebuffer[i] = pgm_read_word(&_bgTransition.to[i]);
    }
    return;
  }
  
  // Calculate blend alpha
  uint8_t alpha = (elapsed * 255) / _bgTransition.duration;
  
  // Blend the two backgrounds
  const uint16_t* fromBg = _bgTransition.from;
  const uint16_t* toBg = _bgTransition.to;
  
  for (uint8_t y = 0; y < 64; y++) {
    for (uint8_t x = 0; x < 64; x++) {
      uint16_t idx = y * 64 + x;
      uint16_t colorFrom = pgm_read_word(&fromBg[idx]);
      uint16_t colorTo = pgm_read_word(&toBg[idx]);
      uint16_t blended = blend565(colorFrom, colorTo, alpha);
      framebuffer[idx] = blended;
    }
  }
}

// ==================== Time Rendering ====================

void StoryClockface::renderTimeLayer() {
  if (!_dateTime) return;
  
  Act* act = _theme->getAct(_currentActId);
  if (!act) return;
  
  drawTime(_dateTime->getHour(), _dateTime->getMinute(), act->getFontColor());
}

void StoryClockface::drawTime(uint8_t hour, uint8_t minute, uint16_t color) {
  fbGfx.setTextSize(1);
  fbGfx.setTextColor(color);

  // Placement
  int x = 10; // x_start
  int y = 34; // y_start

  fbGfx.setCursor(x, y);

  char buf[6];
  bool showColon = (_now / 1000) % 2; // blink every second

  snprintf(buf, sizeof(buf),
           showColon ? "%02d:%02d" : "%02d %02d",
           hour, minute);

  fbGfx.print(buf);
}

// ==================== Text Rendering ====================

void StoryClockface::renderTextLayer() {
  uint32_t elapsed = _now - _text.phaseStart;
  
  Act* act = _theme->getAct(_currentActId);
  if (!act) return;
  
  uint16_t color = act->getFontColor();

  switch (_text.phase) {

    case TEXT_IDLE:
      _text.phrase = act->getNewPhrase();
      if (!_text.phrase) return;
      _text.phase = TEXT_FADE_IN;
      _text.phaseStart = _now;
      break;

    case TEXT_FADE_IN: {
      uint8_t alpha = (elapsed * 255) / TEXT_FADE_MS;
      if (alpha > 255) alpha = 255;
      drawPhraseBlended(_text.phrase, color, alpha);

      if (elapsed >= TEXT_FADE_MS) {
        _text.phase = TEXT_HOLD;
        _text.phaseStart = _now;
      }
    } break;

    case TEXT_HOLD:
      drawPhraseBlended(_text.phrase, color, 255);

      if (elapsed >= TEXT_HOLD_MS) {
        _text.phase = TEXT_FADE_OUT;
        _text.phaseStart = _now;
      }
      break;

    case TEXT_FADE_OUT: {
      uint8_t alpha = 255 - ((elapsed * 255) / TEXT_FADE_MS);
      if (alpha > 255) alpha = 0;  // Handle underflow
      drawPhraseBlended(_text.phrase, color, alpha);

      if (elapsed >= TEXT_FADE_MS) {
        _text.phase = TEXT_QUIET;
        _text.phaseStart = _now;
      }
    } break;

    case TEXT_QUIET:
      if (elapsed >= TEXT_QUIET_MS) {
        _text.phase = TEXT_IDLE;
      }
      break;
  }
}

void StoryClockface::drawPhraseBlended(const char* phrase, uint16_t textColor, uint8_t alpha) {
  if (!phrase) return;

  int w = textWidth(phrase);
  int x = (64 - w) / 2;
  int y = TEXT_Y;

  for (size_t i = 0; i < strlen(phrase); i++) {
    drawCharBlended(phrase[i], x, y, textColor, alpha);
    x += FONT_W + CHAR_SPACING;
  }
}

void StoryClockface::drawCharBlended(char c, int x, int y, uint16_t color, uint8_t alpha) {
  uint8_t index = fontIndex(c);
  const uint8_t* glyph = dune_font5x7[index];

  for (int col = 0; col < FONT_W; col++) {
    uint8_t bits = pgm_read_byte(&glyph[col]);

    for (int row = 0; row < FONT_H; row++) {
      if (bits & (1 << row)) {
        int px = x + col;
        int py = y + row;

        if (px >= 0 && px < 64 && py >= 0 && py < 64) {
          uint16_t bg = fbGet(px, py);
          uint16_t blended = blend565(bg, color, alpha);
          fbSet(px, py, blended);
        }
      }
    }
  }
}

int StoryClockface::textWidth(const char* s) {
  if (!s) return 0;
  return strlen(s) * (FONT_W + CHAR_SPACING) - CHAR_SPACING;
}

}  // namespace dune
