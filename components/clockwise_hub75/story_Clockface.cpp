#include "dune_StoryClockface.h"
#include "dune_act.h"
#include "esphome/core/log.h"

namespace dune {

class FB_GFX : public Adafruit_GFX {
public:
    FB_GFX() : Adafruit_GFX(64, 64) {}

    void drawPixel(int16_t x, int16_t y, uint16_t color) override {
        if (x < 0 || y < 0 || x >= 64 || y >= 64) return;
        Clockface::framebuffer[y * 64 + x] = color;
    }
};

static FB_GFX fbGfx;

uint16_t Clockface::framebuffer[64 * 64];


static const char *const TAG = "StoryClockface";

StoryClockface::StoryClockface(IStoryTheme* theme, GFXWrapper* gfx) {
// TOOD intialize members like _eventbus, _dateTime, etc. to nullptr here and create them in setup() to avoid initialization order issues
  // : theme_(theme), gfx_(gfx), currentActId_(0), frameCount_(0),
  //   currentPhrase_(nullptr), dt_(nullptr) {
  if (!theme) {
    ESP_LOGE(TAG, "Theme cannot be null");
  }
  _theme = theme;
  _display = gfx;
  _currentActId = 0;
  _frameCount = 0;
  _currentPhrase = nullptr;
  _dateTime = nullptr;

  // not sure if this is needed, but we had this also in the dune Clockface constructor
  Locator::provide(_display);
  
  // Initialize pointers to nullptr - objects will be created in setup()
  _eventBus = nullptr;
  _dateTime = nullptr;
}

StoryClockface::~StoryClockface() {
  // Don't delete theme_ - caller owns it
    delete _eventBus;
}

void StoryClockface::setup(CWDateTime* dt) {
  _dateTime = dt;
  if (!_dateTime) {
    ESP_LOGE(TAG, "CWDateTime pointer cannot be null");
    return;
  }
  
    // Create objects here instead of in constructor to avoid initialization order issues
    _eventBus = new EventBus();
    // Provide EventBus after creation
    Locator::provide(_eventBus);

  // Initialize with current act
  _currentActId = _theme->getCurrentActId(*_dateTime);
  Act* act = _theme->getAct(_currentActId);
  if (act) {
    _currentPhrase = act->getNewPhrase();
    ESP_LOGI(TAG, "StoryClockface initialized - Theme: %s, Act: %s",
             _theme->getThemeName(), act->getName());
  }
}

void Clockface::flushFramebuffer() {
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
    _currentActId = newActId;
    Act* act = _theme->getAct(_currentActId);
    if (act) {
      ESP_LOGI(TAG, "Act changed: %s", act->getName());
      // Get first phrase for new act
      _currentPhrase = act->getNewPhrase();
    }
  }
}

void StoryClockface::updatePhrase() {
  Act* act = _theme->getAct(_currentActId);
  if (act) {
    _currentPhrase = act->getNewPhrase();
  }
}

void StoryClockface::renderFrame() {
  // Layer 0: Clear
  _display->fillScreen(0x0000);  // Black

  // Layer 1: Background (theme-specific)
  _theme->renderBackground(gfx, _currentActId);

  // Layer 2: Ambient effects (theme-specific)
  _theme->renderAmbientEffect(gfx, _currentActId, _frameCount);

  // Layer 3: Major events (placeholder for future)
  // _theme->renderMajorEvents(gfx, _currentActId, _frameCount);

  // Layer 4: Time display (theme-specific)
  _theme->renderTimeDisplay(gfx, _currentActId, dt);

  // Layer 5: Text overlay (theme-specific)
  if (_currentPhrase) {
    _theme->renderTextField(gfx, _currentActId, _currentPhrase);
  }
}

}  // namespace dune
