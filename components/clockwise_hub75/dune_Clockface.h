#pragma once

#include <Adafruit_GFX.h>
#include "Tile.h"
#include "Locator.h"
#include "Game.h"
#include "Object.h"
#include "ImageUtils.h"
// Commons
#include "IClockface.h"
#include "CWDateTime.h"

#include "dune_assets.h"
#include "dune_font.h"

namespace dune {

// Array of pointers to background images - using only images from first half to avoid duplicates
static const uint16_t* backgroundImages[10] = {
  dune_baron64x64,
  dune_baron_desert64x64,
  dune_baron_chamber64x64,
  dune_ornithopter64x64,
  dune_chani64x64,
  dune_sandworm64x64,
  dune_paul_sandworm64x64,
  dune_background64x64,
  dune_background_264x64,
  dune_baron_chamber_background64x64
};

//  Global timing constants
#define TEXT_MIN_INTERVAL_MS   600000  // 10 minutes
#define TEXT_DISPLAY_MS        2200
#define TEXT_FADE_MS            400
/*
Event priority (highest wins)
*Act change
*Hour change
*Storm / Worm
*Manual wake
*Ignored
If a higher priority event happens:
*Lower one is discarded
*No queueing


Hard rules (do not break)
* Never show text:
  *Within 2s of minute change
  *During colon blink transition
*Never overlap text
*Never repeat same phrase twice in a row


Timing summary (human-friendly)
| Event       | When              |
| ----------- | ----------------- |
| Act change  | Immediately       |
| Hour change | +500 ms           |
| Storm       | Once per storm    |
| Idle        | Max once / 10 min |


 */

class Clockface : public IClockface {
  private:
    Adafruit_GFX* _display;
    CWDateTime* _dateTime;

    EventBus* eventBus;

    void drawTime(uint8_t hour, uint8_t minute, uint8_t act);
    void drawDigit(uint8_t digit, int x, int y, uint16_t color);
    void drawColon(int x, int y, bool blink, uint16_t color);      
    uint8_t getActForHour(uint8_t hour);
    const char* selectPhrase(uint8_t act);

public:
    Clockface(Adafruit_GFX* display);
    ~Clockface();
    void setup(CWDateTime *dateTime);
    void update();
    void externalEvent(int type);
 
};
}  // namespace dune
