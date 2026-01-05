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
#include "dune_phrases.h"
#include "dune_act.h"

namespace dune {

// // Array of pointers to background images - using only images from first half to avoid duplicates
// static const uint16_t* backgroundImages[10] = {
//   dune_baron64x64,
//   dune_baron_desert64x64,
//   dune_baron_chamber64x64,
//   dune_ornithopter64x64,
//   dune_chani64x64,
//   dune_sandworm64x64,
//   dune_paul_sandworm64x64,
//   dune_background64x64,
//   dune_background_264x64,
//   dune_baron_chamber_background64x64
// };

//  Global timing constants
#define TEXT_MIN_INTERVAL_MS   600000  // 10 minutes
#define TEXT_DISPLAY_MS        2200
#define TEXT_FADE_MS            400

#define WIPE_STEP_MS   30
#define WIPE_COL_JITTER 1


class Clockface : public IClockface {
  private:

    #define COLOR_SHADOW_DARK  0x4208  // dark brown/gray
    #define COLOR_SHADOW_SOFT  0x630C  // softer edge

    typedef enum {
      VS_IDLE,
      VS_ENTER,
      VS_ACTIVE,
      VS_EXIT
    } VisualPhase;

    #define STORM_ENTER_MS   800
    #define STORM_ACTIVE_MS 3000
    #define STORM_EXIT_MS   1200

    typedef enum {
      EVENT_NONE = 0,
      EVENT_STORM,
      EVENT_WORM,
      EVENT_FLIGHT
    } EventType;

    typedef struct {
      bool active = false;
      EventType type = EVENT_NONE;
      VisualPhase phase = VS_IDLE;
      uint32_t phaseStart = 0;
    } Event;

    #define ACT_I 1
    #define ACT_II 2
    #define ACT_III 3
    #define ACT_IV 4
    #define ACT_V 5
    #define ACT_VI 6

    Event _event;

 

    inline void fbClear(uint16_t color);
    inline uint16_t fbGet(uint8_t x, uint8_t y);
    inline void fbSet(uint8_t x, uint8_t y, uint16_t color);


    uint32_t _shadowLastUpdate = 0;
    uint8_t  _shadowX = 0;
    uint8_t  _shadowY = 10;
    int8_t   _shadowDx = 1;

    // Tremor state
    uint32_t _tremorLastUpdate = 0;
    static const uint8_t TREMOR_UPDATE_MS = 100; // 10 fps for ripples

    // time state
    uint32_t _lastHour = 0;
    uint32_t _lastMinute = 0;

    Adafruit_GFX* _display;
    CWDateTime* _dateTime;
    Act _activeAct;
    Act _acts[6];

    uint32_t _lastPhraseUpdate = 0;
    
    uint32_t _now;

    EventBus* _eventBus;

    void drawTime(uint8_t hour, uint8_t minute, uint16_t color);
    void drawDigit(uint8_t digit, int x, int y, uint16_t color);
    void drawColon(int x, int y, bool blink, uint16_t color);

    Act getCurrentAct(uint8_t hour);
    void drawPhraseWithSandWipe(const char* phrase, uint16_t color);
    void initializeActs();
    void drawBackgroundImage(const uint16_t* image);

    void flushFramebuffer();



    void layer_clear();
    void layer_background();
    void layer_ambient();
    void layer_event();
    void layer_time();
    void layer_text();
 
    void ambient_heat();
    void ambient_spice();
    void ambient_shadow();
    void ambient_tremor();
    void ambient_wind();
    void ambient_dust();

    void drawStorm();
    void drawWorm();
    void drawFlight();

    void drawShadowBand(uint8_t xStart, uint8_t yStart);
    uint16_t darken(uint16_t color);
    uint16_t darken(uint16_t color, float factor);
    void drawTremorRipple(uint8_t xStart, uint8_t yStart, const uint16_t* bg);

public:
    Clockface(Adafruit_GFX* display);
    ~Clockface();
    void setup(CWDateTime *dateTime);
    void update();
    void externalEvent(int type);

    static uint16_t framebuffer[64 * 64];    
};
}  // namespace dune
