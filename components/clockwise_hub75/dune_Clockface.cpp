// Minimal implementation - all functionality moved to header for guaranteed linking
#include "dune_Clockface.h"
#include <Adafruit_GFX.h>
#include "Macros.h"

static const char *const TAG = "dune_Clockface";

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

Clockface::Clockface(Adafruit_GFX* display) {
  _display = display;
  Locator::provide(_display);
  
  // Initialize pointers to nullptr - objects will be created in setup()
  _eventBus = nullptr;
  _dateTime = nullptr;
}

Clockface::~Clockface() {
  delete _eventBus;
}

Act Clockface::getCurrentAct(uint8_t hour) {
	// 6 phases, each 4 hours
    uint8_t îdx = hour / 4; // 0-5
    return _acts[îdx];
}

void Clockface::setup(CWDateTime *dateTime) {
	_dateTime = dateTime;

    // Create objects here instead of in constructor to avoid initialization order issues
    _eventBus = new EventBus();
    // Provide EventBus after creation
    Locator::provide(_eventBus);

    initializeActs();
    _activeAct = Act();  // Default empty act

    _event.active = false;
    _event.type = EVENT_NONE;
}

void Clockface::initializeActs() {
	ESP_LOGD(TAG, "initializeActs() called");
	_acts[0] = Act(ACT_I, "The Desert Sleeps", PHRASES_TIME, DIM_SAND, dune_act1);
	_acts[1] = Act(ACT_II, "Spice Awakens", PHRASES_DESERT, SPICE_AMBER, dune_act2);
	_acts[2] = Act(ACT_III, "The Watchers", PHRASES_POWER, HIGH_CONTRAST_WHITE, dune_act3);
	_acts[3] = Act(ACT_IV, "The Maker Stirs", PHRASES_DANGER, BRIGHT_SAND, dune_act4);
	_acts[4] = Act(ACT_V, "Storm of Fate", PHRASES_DANGER, RED_DANGER, dune_act5);
	_acts[5] = Act(ACT_VI, "Silence & Survival", PHRASES_SURVIVAL, COOL_BROWN, dune_act6);
    ESP_LOGD(TAG, "initializeActs() done");
}

void Clockface::flushFramebuffer() {
    _display->drawRGBBitmap(0, 0, framebuffer, 64, 64);
}


void Clockface::update() {
	if (!_dateTime) {
		ESP_LOGE(TAG, "update() failed: _dateTime is nullptr");
		return;
	}
	if (!_display) {
		ESP_LOGE(TAG, "update() failed: _display is nullptr");
		return;
	}
	
    _now = millis();
	_activeAct = getCurrentAct(_dateTime->getHour());
    _activeAct = _acts[0]; // FOR TESTING ONLY - FORCE ACT I
    
    bool text_is_active = (_now - _lastPhraseUpdate) < TEXT_DISPLAY_MS;
    bool event_is_active = false; // Placeholder for event logic

    layer_clear();        // L0 Clear / sky
    layer_background();   // L1 Act background
    layer_ambient();      // L2 Ambient effects
    layer_event();        // L3 Major event overlay (Storm / Worm / Flight object)
    layer_time();         // L4 Time (HH:MM)
    layer_text();         // L5 Text overlay (phrases)

    flushFramebuffer();

	// if (act.getId() != _activeAct.getId()) {
	// 	ESP_LOGD(TAG, "Act changed from %d to %d", _activeAct.getId(), act.getId());
	// 	_activeAct = act;
    //     drawBackgroundImage(_activeAct.getBackground());  // moved to layer_background()
	// }

	// Draw time (HH:MM) using 5x7 font, scaled ×2, at (x=10, y=34)
	//drawTime(hour, minute, _activeAct.getFontColor()); // mooved to layer_time()
    // Update phrase every 10 seconds
    //if (now - _lastPhraseUpdate > 10000) {
        //_lastPhraseUpdate = now;
	    //const char* phrase = _activeAct.getNewPhrase();
	    //printPhrase(phrase); // moved to layer_text()
    //}
}

const unsigned short COOL_BLACK = 0x18C3; // Dark, cool black for clearing

void Clockface::layer_clear() {
    fbClear(COOL_BLACK);
}

void Clockface::layer_background() {
    const uint16_t* bg = _activeAct.getBackground();
	if (!bg) {
		ESP_LOGE(TAG, "layer_background() failed: bg is nullptr");
		return;
	} 

    memcpy(framebuffer, bg, 64 * 64 * sizeof(uint16_t));
   //_display->drawRGBBitmap(0, 0, bg, 64, 64);
}

void Clockface::layer_ambient() {
    switch (_activeAct.getId()) {
        case ACT_I: ambient_heat(); break;
        case ACT_II: ambient_spice(); break;
        case ACT_III: ambient_shadow(); break;
        case ACT_IV: ambient_tremor(); break;
        case ACT_V: ambient_wind(); break;
        case ACT_VI: ambient_dust(); break;
    }
}

void Clockface::layer_event() {
    if (!_event.active) return;

    if (_event.type == EVENT_STORM) {
        drawStorm();
    } else if (_event.type == EVENT_WORM) {
        drawWorm();
    } else if (_event.type == EVENT_FLIGHT) {
        drawFlight();
    }
}

void Clockface::layer_time() {
  bool highContrast = (_event.active && _event.type == EVENT_STORM); // event overrides color, not act

  drawTime(
    _dateTime->getHour(),
    _dateTime->getMinute(),
    highContrast ? HIGH_CONTRAST_WHITE : _activeAct.getFontColor()
  );
}

void Clockface::layer_text() {

    // Global silencing rules
    if (eventSilencesText()) return;
    //if (_nowMs - _lastMinuteChangeMs < 2000) return;

    uint32_t elapsed = _now - _textPhaseStartMs;
    uint16_t baseColor = _activeAct.getFontColor();

    switch (_textPhase) {

        case TEXT_IDLE: {
            const char* phrase = _activeAct.getNewPhrase();
            if (!phrase) return;

           //if (_currentPhrase && strcmp(phrase, _currentPhrase) == 0) return;

            _currentPhrase = phrase;
            _textPhase = TEXT_FADE_IN;
            _textPhaseStartMs = _now;
            break;
        }

        case TEXT_FADE_IN: {
            uint8_t alpha = MIN(255, (elapsed * 255) / TEXT_FADE_IN_MS);
            drawPhraseBlended(
                _currentPhrase,
                baseColor, 
                alpha
            );

            if (elapsed >= TEXT_FADE_IN_MS) {
                _textPhase = TEXT_HOLD;
                _textPhaseStartMs = _now;
            }
            break;
        }

        case TEXT_HOLD:
            drawPhraseBlended(_currentPhrase, baseColor, 255);

            if (elapsed >= TEXT_HOLD_MS) {
                _textPhase = TEXT_FADE_OUT;
                _textPhaseStartMs = _now;
            }
            break;

        case TEXT_FADE_OUT: {
            uint8_t alpha = 255 - MIN(255, (elapsed * 255) / TEXT_FADE_OUT_MS);
            drawPhraseBlended(
                _currentPhrase,
                baseColor,
                 alpha
            );

            if (elapsed >= TEXT_FADE_OUT_MS) {
                _textPhase = TEXT_QUIET;
                _textPhaseStartMs = _now;
            }
            break;
        }

        case TEXT_QUIET:
            if (elapsed >= TEXT_QUIET_MS) {
                _textPhase = TEXT_IDLE;
                _textPhaseStartMs = _now;
            }
            break;
    }
}

// void Clockface::layer_text() {
//     // Rule 1: never show text during silencing events
//     if (eventSilencesText()) return;

//     // Rule 2: never show text within 2s of minute change
//     //if (_now - _lastMinuteChange < 2000) return;

//     uint32_t elapsed = _now - _textPhaseStartMs;
//     uint16_t baseColor = _activeAct.getFontColor();

//     // Rule 3: if text is active, keep showing it
//     if (_textActive) {
//         drawPhraseWithSandWipe(_currentPhrase, _activeAct.getFontColor());

//         // End text after duration
//         if (_now - _textStartTime > TEXT_DURATION_MS) {
//             _textActive = false;
//         }
//         return;
//     }

//     // Rule 4: only request a new phrase when idle
//     const char* phrase = _activeAct.getNewPhrase();
//     if (!phrase) return;

//     // Rule 5: never repeat same phrase twice -> this is already handled in Act::getNewPhrase()

//     // Activate new phrase
//     _currentPhrase = phrase;
//     _textStartTime = _now;
//     _textActive = true;

//     drawPhraseWithSandWipe(_currentPhrase, _activeAct.getFontColor());
// }


void Clockface::ambient_heat() {
    // Placeholder for heat ambient effect
}
void Clockface::ambient_spice() {
    // Placeholder for spice ambient effect
}
void Clockface::ambient_shadow() {
    if (_now - _shadowLastUpdate < 80) return;
    _shadowLastUpdate = _now;

    _shadowX += _shadowDx;

    if (_shadowX > 48 || _shadowX < 4) {
        _shadowDx = -_shadowDx;
        _shadowY = 8 + (_shadowX % 16);
    }

    ESP_LOGD(TAG, "shadowX=%d now=%lu", _shadowX, _now);

    drawShadowBand(_shadowX, _shadowY);
}
void Clockface::ambient_tremor() {
    if (_now - _tremorLastUpdate < TREMOR_UPDATE_MS) return;
    _tremorLastUpdate = _now;

    const uint16_t* bg = _activeAct.getBackground();

    // Draw 8–12 small ripples per frame
    for (uint8_t i = 0; i < 10; i++) {
        // Random-ish positions using now for deterministic pseudo-random
        uint8_t y = 32 + ((i * 7 + _now / 150) % 20); // avoid top-center (0–31)
        uint8_t x = (i * 5 + (_now / 100)) % 48;      // ripple within safe width

        drawTremorRipple(x, y, bg);
    }
}
void Clockface::ambient_wind() {
    // Placeholder for wind ambient effect
}
void Clockface::ambient_dust() {
    // Placeholder for dust ambient effect
}

void Clockface::drawStorm() {
    // Placeholder for storm drawing
}
void Clockface::drawWorm() {
    // Placeholder for worm drawing
}
void Clockface::drawFlight() {
    // Placeholder for flight drawing
}

void Clockface::drawShadowBand(uint8_t xStart, uint8_t yStart) {
    const uint8_t width = 16;
    const uint8_t height = 6;
    
    const uint16_t* bg = _activeAct.getBackground();

    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {

            uint8_t px = xStart + x;
            uint8_t py = yStart + y;
            if (px >= 64 || py >= 64) continue;

            uint16_t base = bg[py * 64 + px];
            uint16_t shaded = darken(base);

            fbGfx.drawPixel(px, py, shaded);
        }
    }
}

uint16_t Clockface::darken(uint16_t color) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;

    r = (r * 3) / 4;
    g = (g * 3) / 4;
    b = (b * 3) / 4;

    return (r << 11) | (g << 5) | b;
}

uint16_t Clockface::darken(uint16_t color, float factor) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;

    r = (uint8_t)(r * factor);
    g = (uint8_t)(g * factor);
    b = (uint8_t)(b * factor);

    return (r << 11) | (g << 5) | b;
}


void Clockface::drawTremorRipple(uint8_t xStart, uint8_t yStart, const uint16_t* bg) {
    const uint8_t width = 8;
    const uint8_t height = 2;

    for (uint8_t y = 0; y < height; y++) {
        for (uint8_t x = 0; x < width; x++) {
            uint8_t px = xStart + x;
            uint8_t py = yStart + y;
            if (px >= 64 || py >= 64) continue;

            // Sample background
            uint16_t base = bg[py * 64 + px];
            // Darken for ripple
            uint16_t ripple = darken(base, 0.8f); // darken 20%

            fbGfx.drawPixel(px, py, ripple);
        }
    }
}

void Clockface::drawPhraseBlended(const char* phrase, uint16_t color, uint8_t alpha) {
    if (!phrase) {
        return;
    }

    if (alpha < 16) alpha = 16;

    // Display phrase at top (y=2), centered
    fbGfx.setTextSize(1); // Default size
    fbGfx.setTextColor(color); // Use passed color
    int16_t x1, y1;
    uint16_t w, h;
    fbGfx.getTextBounds(phrase, 0, 0, &x1, &y1, &w, &h);
    int x = (64 - w) / 2;
    fbGfx.setCursor(x, 2);

    // Draw each character with blending
    for (size_t i = 0; i < strlen(phrase); i++) {
        char c = phrase[i];
        int16_t cx = fbGfx.getCursorX();
        int16_t cy = fbGfx.getCursorY();

        // Draw character to temporary buffer
        fbGfx.print(c);
        int16_t nx = fbGfx.getCursorX();

        // Blend character pixels
        for (int16_t px = cx; px < nx; px++) {
            for (int16_t py = cy; py < cy + 8; py++) { // assuming 8 pixel height
                uint16_t fg = fbGet(px, py);
                uint16_t bg = Clockface::framebuffer[py * 64 + px];
                uint16_t blended = blend565(bg, fg, alpha);
                Clockface::framebuffer[py * 64 + px] = blended;
            }
        }
    }
}

void Clockface::drawPhraseWithSandWipe(const char* phrase, uint16_t color) {
    if (!phrase) {
		return;
	}

	// Display phrase at top (y=2), centered
	fbGfx.setTextSize(1); // Default size
	fbGfx.setTextColor(color); // Use passed color
	int16_t x1, y1;
	uint16_t w, h;
	fbGfx.getTextBounds(phrase, 0, 0, &x1, &y1, &w, &h);
	int x = (64 - w) / 2;
	fbGfx.setCursor(x, 2);
	fbGfx.print(phrase);
}

void Clockface::drawTime(uint8_t hour, uint8_t minute, uint16_t color) {
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

void Clockface::externalEvent(int type) {
	// TODO: Handle external events (storm, worm, etc.)
}


inline void Clockface::fbClear(uint16_t color) {
    for (uint16_t i = 0; i < 64 * 64; i++) {
        framebuffer[i] = color;
    }
}

inline uint16_t Clockface::fbGet(uint8_t x, uint8_t y) {
    return framebuffer[y * 64 + x];
}

inline void Clockface::fbSet(uint8_t x, uint8_t y, uint16_t color) {
    framebuffer[y * 64 + x] = color;
}

bool Clockface::eventSilencesText() const {
    if (!_event.active) return false;

    switch (_event.type) {
        case EVENT_STORM:
        case EVENT_WORM:
            return true;
        default:
            return false;
    }
}

uint16_t Clockface::fadeColor(uint16_t color, uint8_t alpha) {
    // alpha: 0–255
    uint8_t r = ((color >> 11) & 0x1F) * alpha / 255;
    uint8_t g = ((color >> 5)  & 0x3F) * alpha / 255;
    uint8_t b = ( color        & 0x1F) * alpha / 255;

    return (r << 11) | (g << 5) | b;
}

uint16_t Clockface::blend565(uint16_t bg, uint16_t fg, uint8_t alpha) {

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



}  // namespace dune
