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
    return acts[îdx];
}

void Clockface::setup(CWDateTime *dateTime) {
	_dateTime = dateTime;

  // Create objects here instead of in constructor to avoid initialization order issues
  _eventBus = new EventBus();
  // Provide EventBus after creation
  Locator::provide(_eventBus);

  initializeActs();

  updateTime();  
  _act = Act(); 
}

void Clockface::initializeActs() {
	ESP_LOGD(TAG, "initializeActs() called");
	acts[0] = Act(1, "The Desert Sleeps", PHRASES_TIME, DIM_SAND, dune_moons64x64);
	acts[1] = Act(2, "Spice Awakens", PHRASES_DESERT, SPICE_AMBER, dune_background64x64);
	acts[2] = Act(3, "The Watchers", PHRASES_POWER, HIGH_CONTRAST_WHITE, dune_ornithopter64x64);
	acts[3] = Act(4, "The Maker Stirs", PHRASES_DANGER, BRIGHT_SAND, dune_sandworm64x64);
	acts[4] = Act(5, "Storm of Fate", PHRASES_DANGER, RED_DANGER, dune_paul_sandworm64x64);
	acts[5] = Act(6, "Silence & Survival", PHRASES_SURVIVAL, COOL_BROWN, dune_chani64x64);
    ESP_LOGD(TAG, "initializeActs() done");
}

void Clockface::updateTime() {
  ESP_LOGD(TAG, "updateTime() called - Hour: %d, Minute: %02d", _dateTime->getHour(), _dateTime->getMinute());
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
	
    uint32_t now_ms = millis();
	uint8_t current_hour = _dateTime->getHour();
	uint8_t current_minute = _dateTime->getMinute();
	Act current_act = getCurrentAct(current_hour);
    bool colon_visible = (now_ms / 1000) % 2 == 0;
    bool text_is_active = (now_ms - _lastPhraseUpdate) < TEXT_DISPLAY_MS;
    bool event_is_active = false; // Placeholder for event logic

    RenderContext ctx = {
        .now_ms = now_ms,
        .act = current_act,
        .hour = current_hour,
        .minute = current_minute,
        .colon_on = colon_visible,
        .text_active = text_is_active,
        .event_active = event_is_active
    };

    layer_clear(&ctx);        // L0
    layer_background(&ctx);   // L1
    layer_ambient(&ctx);      // L2
    layer_event(&ctx);        // L3
    layer_time(&ctx);         // L4
    layer_text(&ctx);         // L5

    //display_swap();           // push framebuffer

	// if (act.getId() != _act.getId()) {
	// 	ESP_LOGD(TAG, "Act changed from %d to %d", _act.getId(), act.getId());
	// 	_act = act;
    //     drawBackgroundImage(_act.getBackground());  // moved to layer_background()
	// }

	// Draw time (HH:MM) using 5x7 font, scaled ×2, at (x=10, y=34)
	//drawTime(hour, minute, _act.getFontColor()); // mooved to layer_time()

    // Update phrase every 10 seconds
    if (now - _lastPhraseUpdate > 10000) {
        _lastPhraseUpdate = now;
	    const char* phrase = _act.getNewPhrase();
	    //printPhrase(phrase); // moved to layer_text()
    }
}


void Clockface::layer_clear(const RenderContext* ctx) {
  // Fill framebuffer
  // Use night color if act == I or VI
  //display_fill(COLOR_SKY);
  //framebuffer_fill(_acts[ctx->act_index].getBaseColor());
}

void Clockface::layer_background(const RenderContext* ctx) {
  const uint16_t* bg = _acts[ctx->act_index].getBackground();
   _display->drawRGBBitmap(0, 0, bg, 64, 64);
}

void Clockface::layer_ambient(const RenderContext* ctx) {
  //_acts[ctx->act_index];
  switch (ctx->act_index) {
    // case ACT_I: ambient_heat(ctx); break;
    // case ACT_II: ambient_spice(ctx); break;
    // case ACT_III: ambient_shadow(ctx); break;
    // case ACT_IV: ambient_tremor(ctx); break;
    // case ACT_V: ambient_wind(ctx); break;
    // case ACT_VI: ambient_dust(ctx); break;
  }
}

void Clockface::layer_event(const RenderContext* ctx) {
  if (!ctx->event_active) return;
/*
  switch (event.type) {
    case EVENT_STORM:
      draw_storm(ctx);
      break;

    case EVENT_WORM:
      draw_worm(ctx);
      break;

    case EVENT_FLIGHT:
      draw_flight(ctx);
      break;
  }
*/      
}

void Clockface::layer_time(const RenderContext* ctx) {
  bool high_contrast = ctx->event_active || ctx->act_index == 5; // Storm of Fate

  drawtime(
    ctx->hour,
    ctx->minute,
    //ctx->colon_on,
    _acts[ctx->act_index].getFontColor()
  );
}

void Clockface::layer_text(const RenderContext* ctx) {
  if (!ctx->text_active) return;

  draw_phrase_with_wipe(ctx->now_ms);
}



void Clockface::printPhrase(const char* phrase) {
    if (!phrase) {
		return;
	}

	// Display phrase at top (y=2), centered
	_display->setTextSize(1); // Default size
	_display->setTextColor(0xFFFF); // White for visibility
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
