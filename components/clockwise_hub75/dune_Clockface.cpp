// Minimal implementation - all functionality moved to header for guaranteed linking
#include "dune_Clockface.h"

static const char *const TAG = "dune_Clockface";

namespace dune {

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

void Clockface::update() {
	if (!_dateTime) {
		ESP_LOGE(TAG, "update() failed: _dateTime is nullptr");
		return;
	}
	if (!_display) {
		ESP_LOGE(TAG, "update() failed: _display is nullptr");
		return;
	}
	
    uint32_t now = millis();
	_activeAct = getCurrentAct(_dateTime->getHour());
    _activeAct = _acts[0]; // FOR TESTING ONLY - FORCE ACT I
    
    bool text_is_active = (now - _lastPhraseUpdate) < TEXT_DISPLAY_MS;
    bool event_is_active = false; // Placeholder for event logic

    layer_clear();        // L0 Clear / sky
    layer_background();   // L1 Act background
    layer_ambient();      // L2 Ambient effects
    layer_event();        // L3 Major event overlay (Storm / Worm / Flight object)
    layer_time();         // L4 Time (HH:MM)
    layer_text();         // L5 Text overlay (phrases)

    //display_swap();           // push framebuffer

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


void Clockface::layer_clear() {
    // not needed with direct drawing
    //_display->fillScreen(COLOR_SKY_DARK);
}

void Clockface::layer_background() {
    const uint16_t* bg = _activeAct.getBackground();
	if (!bg) {
		ESP_LOGE(TAG, "layer_background() failed: bg is nullptr");
		return;
	} 
   _display->drawRGBBitmap(0, 0, bg, 64, 64);
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
    if (_event.active) return;   // events silence text

    const char* phrase = _activeAct.getNewPhrase();
    if (phrase) {
        drawPhraseWithSandWipe(phrase, _activeAct.getFontColor());
    }
}


void Clockface::ambient_heat() {
    // Placeholder for heat ambient effect
}
void Clockface::ambient_spice() {
    // Placeholder for spice ambient effect
}
void Clockface::ambient_shadow() {
    uint32_t now = millis();
    if (now - _shadowLastUpdate < 80) return;
    _shadowLastUpdate = now;

    _shadowX += _shadowDx;

    if (_shadowX > 48 || _shadowX < 4) {
        _shadowDx = -_shadowDx;
        _shadowY = 8 + (_shadowX % 16);
    }

    ESP_LOGD(TAG, "shadowX=%d now=%lu", _shadowX, now);

    drawShadowBand(_shadowX, _shadowY);
}
void Clockface::ambient_tremor() {
    uint32_t now = millis();
    if (now - _tremorLastUpdate < TREMOR_UPDATE_MS) return;
    _tremorLastUpdate = now;

    const uint16_t* bg = _activeAct.getBackground();

    // Draw 8–12 small ripples per frame
    for (uint8_t i = 0; i < 10; i++) {
        // Random-ish positions using now for deterministic pseudo-random
        uint8_t y = 32 + ((i * 7 + now / 150) % 20); // avoid top-center (0–31)
        uint8_t x = (i * 5 + (now / 100)) % 48;      // ripple within safe width

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

            _display->drawPixel(px, py, shaded);
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

            _display->drawPixel(px, py, ripple);
        }
    }
}


void Clockface::drawPhraseWithSandWipe(const char* phrase, uint16_t color) {
    if (!phrase) {
		return;
	}

	// Display phrase at top (y=2), centered
	_display->setTextSize(1); // Default size
	_display->setTextColor(color); // Use passed color
	int16_t x1, y1;
	uint16_t w, h;
	_display->getTextBounds(phrase, 0, 0, &x1, &y1, &w, &h);
	int x = (64 - w) / 2;
	_display->setCursor(x, 2);
	_display->print(phrase);
}

void Clockface::drawDigit(uint8_t digit, int x, int y, uint16_t color) {
	//extern const uint8_t font5x7_digits[][5];
	if (!_display) {
		ESP_LOGE(TAG, "drawDigit() failed: _display is nullptr");
		return;
	}
	if (digit > 9) {
		ESP_LOGE(TAG, "drawDigit() failed: digit out of range: %d", digit);
		return;
	}
	for (int col = 0; col < 5; ++col) {
		uint8_t bits = dune_font5x7_digits[digit][col];
		for (int row = 0; row < 7; ++row) {
			if (bits & (1 << row)) {
				// Draw 2x2 block for scaling
				_display->fillRect(x + col*2, y + row*2, 2, 2, color);
			}
		}
	}
}

// Helper: Draw colon
void Clockface::drawColon(int x, int y, bool blink, uint16_t color) {
	if (!_display) {
		ESP_LOGE(TAG, "drawColon() failed: _display is nullptr");
		return;
	}
	// Colon is 4x14 px, two dots
	if (blink) {
		// Draw upper dot
		_display->fillRect(x, y+4, 4, 4, color);
		// Draw lower dot
		_display->fillRect(x, y+10, 4, 4, color);
	}
}

void Clockface::drawTime(uint8_t hour, uint8_t minute, uint16_t color) {
	if (!_display) {
		ESP_LOGE(TAG, "drawTime() failed: _display is nullptr");
		return;
	}

    if (hour == _lastHour && minute == _lastMinute) {
        // No change, avoids flicker
        return;
    }

    ESP_LOGD(TAG, "drawTime() updating time %02d:%02d", hour, minute);
    _lastHour = hour;
    _lastMinute = minute;

	// Placement
	int x = 10; // x_start
	int y = 34; // y_start

	// Format HH:MM
	uint8_t digits[4] = {
		static_cast<uint8_t>(hour / 10),
		static_cast<uint8_t>(hour % 10),
		static_cast<uint8_t>(minute / 10),
		static_cast<uint8_t>(minute % 10)
	};

	// Draw digits
	for (int i = 0; i < 2; ++i) {
		drawDigit(digits[i], x + i*10, y, color);
	}
	// Colon
	bool blink = (millis() / 1000) % 2 == 0;
	drawColon(x + 20, y, blink, color);
	// Last two digits
	for (int i = 2; i < 4; ++i) {
		drawDigit(digits[i], x + 24 + (i-2)*10, y, color);
	}
}

void Clockface::externalEvent(int type) {
	// TODO: Handle external events (storm, worm, etc.)
}

}  // namespace dune
