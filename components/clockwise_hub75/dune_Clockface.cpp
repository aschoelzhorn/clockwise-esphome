// Minimal implementation - all functionality moved to header for guaranteed linking
#include "dune_Clockface.h"

namespace dune {

// Helper: Get act/phase from hour
uint8_t Clockface::getActForHour(uint8_t hour) {
	// 6 phases, each 4 hours
	return hour / 4; // 0-5
}

Clockface::Clockface(Adafruit_GFX* display) : _display(display), _dateTime(nullptr), eventBus(nullptr) {}

Clockface::~Clockface() {}

void Clockface::setup(CWDateTime *dateTime) {
	_dateTime = dateTime;
}

void Clockface::update() {
	if (!_dateTime) return;
	uint8_t hour = _dateTime->getHour();
	uint8_t minute = _dateTime->getMinute();
	uint8_t act = getActForHour(hour);


	// Act-specific background
	extern const uint16_t* backgroundImages[10];
	// Use act as index, wrap if needed
	const uint16_t* bg = backgroundImages[act % 10];
	// Draw background image (assume 64x64)
	if (bg) {
		// If using Adafruit_GFX, drawBitmap expects (x, y, bitmap, w, h, color)
		// Here, color is ignored for color bitmaps, so use 0
		_display->drawRGBBitmap(0, 0, bg, 64, 64);
	}

	// Draw time (HH:MM) using 5x7 font, scaled ×2, at (x=10, y=34)
	drawTime(hour, minute, act);

	// Select and display phrase for current act
	extern const char* selectPhrase(uint8_t act);
	const char* phrase = selectPhrase(act);
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

// Helper: Draw a single digit using 5x7 font, scaled ×2
void Clockface::drawDigit(uint8_t digit, int x, int y, uint16_t color) {
	extern const uint8_t font5x7_alpha[][5];
	for (int col = 0; col < 5; ++col) {
		uint8_t bits = font5x7_alpha[digit][col];
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
	// Colon is 4x14 px, two dots
	if (blink) {
		// Draw upper dot
		_display->fillRect(x, y+4, 4, 4, color);
		// Draw lower dot
		_display->fillRect(x, y+10, 4, 4, color);
	}
}

void Clockface::drawTime(uint8_t hour, uint8_t minute, uint8_t act) {
	// Placement
	int x = 10; // x_start
	int y = 34; // y_start

	// Act-specific color palette
	static const uint16_t actColors[6] = {
		0xC618, // ACT I: Desert Sleeps (dim sand)
		0xFCA0, // ACT II: Spice Awakens (spice amber)
		0xFFFF, // ACT III: Watchers (high contrast, white)
		0xFFE0, // ACT IV: Maker Stirs (bright sand)
		0xF800, // ACT V: Storm of Fate (red, danger)
		0x8410  // ACT VI: Silence & Survival (cool brown)
	};
	uint16_t color = actColors[act % 6];

	// Format HH:MM
	uint8_t digits[4] = {
		hour / 10,
		hour % 10,
		minute / 10,
		minute % 10
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
