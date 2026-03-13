#include "story_Clockface.h"
#include "story_act.h"
#include "esphome/core/log.h"
#include <Adafruit_GFX.h>
#include <pgmspace.h>

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
  uint8_t newActId = _theme->getCurrentActId(dt);
  
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

  // Layer 4: Time display (theme-specific)
  // _theme->renderTimeDisplay(gfx, _currentActId, dt);

  // Layer 5: Text overlay (theme-specific)
  // if (_currentPhrase) {
  //   _theme->renderTextField(gfx, _currentActId, _currentPhrase);
  // }
  
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

}  // namespace dune
