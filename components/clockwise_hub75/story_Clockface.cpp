#include "story_Clockface.h"
#include "story_act.h"
#include "story_font.h"
#include "esphome/core/log.h"
#include <Adafruit_GFX.h>
#include <pgmspace.h>

// Enable test mode to cycle acts every 15 seconds
// #define TEST_ACT_CYCLING

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

StoryClockface::StoryClockface(IStoryTheme* theme, Adafruit_GFX* gfx) {
  if (!theme) {
    ESP_LOGE(TAG, "Theme cannot be null");
  }
  if (!gfx) {
    ESP_LOGE(TAG, "Adafruit_GFX cannot be null");
  }  
  _theme = theme;
  _display = gfx;
  // _currentActId = 0;
  // _frameCount = 0;
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

    _activeAct = Act();  // Default empty act
    //_event.active = false;
    //_event.type = EVENT_NONE;

  // Initialize with current act
  // _currentActId = _theme->getCurrentActId(*_dateTime);
  // Act* act = _theme->getAct(_currentActId);
  // if (act) {
    // _currentPhrase = act->getNewPhrase();
    ESP_LOGI(TAG, "StoryClockface initialized - Theme: %s", _theme->getThemeName());
}

void StoryClockface::flushFramebuffer() {
    _display->drawRGBBitmap(0, 0, framebuffer, 64, 64);
}


void StoryClockface::update() {
  _now = millis();
  
  // Track minute changes for event timing
  uint8_t currentMinute = _dateTime->getMinute();
  if (currentMinute != _lastMinute) {
      _lastMinute = currentMinute;
      _lastMinuteChange = _now;
  }

  setActiveAct(); // Update active Act based on current hour
  bool event_is_active = false; // Placeholder for event logic

  render(_display);
}

void StoryClockface::render(Adafruit_GFX* gfx) {
  if (!_theme || !gfx) {
    return;
  }
  
  layer_clear();        // L0 Clear / sky
  layer_background();   // L1 Act background
  layer_ambient();      // L2 Ambient effects
  layer_event();        // L3 Major event overlay (Storm / Worm / Flight object)
  layer_time();         // L4 Time (HH:MM)
  layer_text();         // L5 Text overlay (phrases)

  flushFramebuffer();
}

void StoryClockface::setActiveAct() {
    Act* act = _theme->getAct(_theme->getCurrentActId(*_dateTime));
    if (act) {
      _activeAct = *act;
    }
}

void StoryClockface::layer_clear() {
  fbClear(0x0000);
}

void StoryClockface::layer_background() {

  // No transition → draw normally
  if (!_bgTransition.active) {
    const uint16_t* bg = _activeAct.getBackground();
	if (!bg) {
		ESP_LOGE(TAG, "layer_background() failed: bg is nullptr");
		return;
	} 
    memcpy(framebuffer, bg, 64 * 64 * sizeof(uint16_t));    
    return;
  }

  // Transition active
  uint32_t elapsed = _now - _bgTransition.start;

  if (elapsed >= _bgTransition.duration) {
    // End transition
    _bgTransition.active = false;

    const uint16_t* bg = _bgTransition.to;
    for (uint8_t y = 0; y < 64; y++) {
      for (uint8_t x = 0; x < 64; x++) {
        fbSet(x, y, bg[y * 64 + x]);
      }
    }
    return;
  }

  uint8_t alpha = (elapsed * 255) / _bgTransition.duration;

  const uint16_t* a = _bgTransition.from;
  const uint16_t* b = _bgTransition.to;

  for (uint8_t y = 0; y < 64; y++) {
    for (uint8_t x = 0; x < 64; x++) {
      uint16_t blended =
        blend565(a[y * 64 + x], b[y * 64 + x], alpha);
      fbSet(x, y, blended);
    }
  }    

}

void StoryClockface::layer_ambient() {
    // Ambient states are enabled in enterAct() based on current act
    // Each act has its own ambient effect implementation
   return; // disable for testing
    // switch (_activeAct.getId()) {
    //     case ACT_I: ambient_heat(); break;
    //     case ACT_II: ambient_spice(); break;
    //     case ACT_III: ambient_shadow(); break;
    //     case ACT_IV: ambient_tremor(); break;
    //     case ACT_V: ambient_wind(); break;
    //     case ACT_VI: ambient_dust(); break;
    // }
}

void StoryClockface::layer_event() {
  return; // disable for testing

  // if (!_event.active) {
  //   maybeStartEvent();
  //   return;
  // }

  // switch (_event.type) {
  
  //   case EVENT_STORM:
  //     drawStorm();
  //     break;

  //   case EVENT_WORM:
  //     drawWorm();
  //     break;

  //   case EVENT_FLIGHT:
  //     drawFlight();
  //     break;

  //   default:
  //     break;
  // }
}

void StoryClockface::layer_time() {
  bool highContrast = false; // event overrides color, not act

  drawTime(
    _dateTime->getHour(),
    _dateTime->getMinute(),
    highContrast ? HIGH_CONTRAST_WHITE : _activeAct.getFontColor()
  );
}

void StoryClockface::layer_text() {
  // if (eventSilencesText()) return;
    //if (_now - _lastMinuteChange < MINUTE_GUARD) return;

    uint32_t elapsed = _now - _text.phaseStart;

    switch (_text.phase) {

        case TEXT_IDLE: 
            _text.phrase = _activeAct.getNewPhrase();
            if (!_text.phrase) return;
            _text.phase = TEXT_FADE_IN;
            _text.phaseStart = _now;
            break;

        case TEXT_FADE_IN: {
          uint8_t alpha = ((elapsed * 255) / TEXT_FADE_MS > 255) ? 255 : (elapsed * 255) / TEXT_FADE_MS;
            drawPhraseBlended(
                _text.phrase,
                _activeAct.getFontColor(), 
                alpha
            );

            if (elapsed >= TEXT_FADE_MS) {
                _text.phase = TEXT_HOLD;
                _text.phaseStart = _now;
            }
        } break;

        case TEXT_HOLD:
            drawPhraseBlended(_text.phrase, _activeAct.getFontColor(), 255);

            if (_now - _text.phaseStart >= TEXT_HOLD_MS) {
                _text.phase = TEXT_FADE_OUT;
                _text.phaseStart = _now;
            }
            break;

        case TEXT_FADE_OUT: {
          uint8_t alpha = 255 - ((((elapsed * 255) / TEXT_FADE_MS) > 255) ? 255 : ((elapsed * 255) / TEXT_FADE_MS));
            drawPhraseBlended(
                _text.phrase,
                _activeAct.getFontColor(),
                 alpha
            );

            if (elapsed >= TEXT_FADE_MS) {
                _text.phase = TEXT_QUIET;
                _text.phaseStart = _now;
            }
        } break;

        case TEXT_QUIET:
            if (_now - _text.phaseStart >= TEXT_QUIET_MS) {
                _text.phase = TEXT_IDLE;
            }
            break;
    }
}

void StoryClockface::enterAct(uint8_t actId) {

  int currentActId = actId;
  int previousActId = currentActId - 1;

  
  if (currentActId < ACT_I) {
    // No previous act, no transition, can never happen normally
    ESP_LOGD(TAG, "enterAct with currentActId < 1 can never happen normally");
    _bgTransition.active = false;
    //memcpy(framebuffer, newBg, 64 * 64 * sizeof(uint16_t));
    return;
  }
  
  if (previousActId == 0) { // wrap around
    previousActId = ACT_VI;
  }

  ESP_LOGD(TAG, "currentActId: %d, previousActId: %d", currentActId, previousActId);

  // act id not identical to array index, but we could fix that by add in a dummy act at index 0
  Act* newAct = _theme->getAct(currentActId - 1);
  Act* oldAct = _theme->getAct(previousActId - 1);
  if (!newAct || !oldAct) return;
  const uint16_t* newBg = newAct->getBackground();
  const uint16_t* oldBg = oldAct->getBackground();

  _bgTransition.active = true;
  _bgTransition.from = oldBg;
  _bgTransition.to = newBg;
  _bgTransition.start = _now;
}


// =================== Events ===================
// void StoryClockface::maybeStartEvent() {

//   // No overlapping events
//   if (_event.active) return;

//   // Guard: don't start near minute change
//   if (_now - _lastMinuteChange < 3000) return;

//   // Simple probability gate (tune later)
//   // if (random(1000) > 2) return; // disable for testing

//   // Act-based selection
//   uint8_t actId = _activeAct.getId();
//    // TODO this comes from the theme
//   // // EVENT_FLIGHT missing    
//   // if (actId >= ACT_IV) {
//   //   startEvent(EVENT_WORM);
//   // } else if (actId >= ACT_II) {
//   //   startEvent(EVENT_STORM);
//   // }
// }

// void StoryClockface::startEvent(EventType type) {
//   _event.active = true;
//   _event.type = type;
//   _event.phase = EP_ENTER;
//   _event.phaseStart = _now;

//   ESP_LOGD(TAG, "Event started: %d", type);
// }

// void StoryClockface::endEvent() {
//   _event.active = false;
//   _event.type = EVENT_NONE;
//   _event.phase = EP_IDLE;

//   ESP_LOGD(TAG, "Event ended");  
// }


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
  // if (alpha < 16) alpha = 16; //TODO check if this is needed - it was in the dune Clockface but not sure if it makes sense here
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

void StoryClockface::drawTime(uint8_t hour, uint8_t minute, uint16_t color) {
    fbGfx.setTextSize(1);
    fbGfx.setTextColor(_activeAct.getFontColor());

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

}  // namespace dune
