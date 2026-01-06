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
//#include "dune_font.h"
#include "dune_phrases.h"
#include "dune_act.h"

namespace dune {

class Clockface : public IClockface {
  private:

    static constexpr int TEXT_Y = 2;
    static constexpr int FONT_W = 5;
    static constexpr int FONT_H = 7;
    static constexpr int CHAR_SPACING = 1;
    int textWidth(const char* s);

    #define COLOR_COOL_BLACK   0x18C3 // Dark, cool black for clearing
    #define COLOR_SHADOW_DARK  0x4208  // dark brown/gray
    #define COLOR_SHADOW_SOFT  0x630C  // softer edge

    // frame_text begin
    // Text state
    enum TextPhase {
        TEXT_IDLE,
        TEXT_FADE_IN,
        TEXT_HOLD,
        TEXT_FADE_OUT,
        TEXT_QUIET
    };

    struct TextState {
      TextPhase phase = TEXT_IDLE;
      const char* phrase = nullptr;
      uint32_t phaseStart = 0;
    };

    static constexpr uint32_t TEXT_FADE_MS  = 1000;
    static constexpr uint32_t TEXT_HOLD_MS     = 3000;
    static constexpr uint32_t TEXT_QUIET_MS    = 1500; //600000; // 10 minutes

    TextState _text;
// frame_text end

// frame ambient begin
struct AmbientState {
  bool enabled = false;
  uint32_t lastUpdate = 0;
  float phase = 0.0f;
};
AmbientState _ambientHeat;
AmbientState _ambientSand;
AmbientState _ambientTremor;


//frame ambient end

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
    Event _event;

    #define ACT_I 1
    #define ACT_II 2
    #define ACT_III 3
    #define ACT_IV 4
    #define ACT_V 5
    #define ACT_VI 6

    // framebuffer for double buffering
    inline void fbClear(uint16_t color);
    inline uint16_t fbGet(uint8_t x, uint8_t y);
    inline void fbSet(uint8_t x, uint8_t y, uint16_t color);

    // Shadow state
    uint32_t _shadowLastUpdate = 0;
    uint8_t  _shadowX = 0;
    uint8_t  _shadowY = 10;
    int8_t   _shadowDx = 1;

    // Tremor state
    uint32_t _tremorLastUpdate = 0;
    static const uint8_t TREMOR_UPDATE_MS = 100; // 10 fps for ripples

    Adafruit_GFX* _display;
    CWDateTime* _dateTime;
    Act _activeAct;
    Act _acts[6];

    uint32_t _now;

    EventBus* _eventBus;

    Act getCurrentAct(uint8_t hour);

    void initializeActs();
    void setActiveAct();
    void enterAct(uint8_t actId);

    void drawBackgroundImage(const uint16_t* image);
    void flushFramebuffer();
    void drawTime(uint8_t hour, uint8_t minute, uint16_t color);
    void drawDigit(uint8_t digit, int x, int y, uint16_t color);
    void drawColon(int x, int y, bool blink, uint16_t color);


    // rendering layers
    void layer_clear();
    void layer_background();
    void layer_ambient();
    void layer_event();
    void layer_time();
    void layer_text();

    // ambient effects
    void ambient_heat();
    void ambient_spice();
    void ambient_shadow();
    void ambient_tremor();
    void ambient_wind();
    void ambient_dust();

    // event effects
    void drawStorm();
    void drawWorm();
    void drawFlight();

    
    void drawShadowBand(uint8_t xStart, uint8_t yStart);
    uint16_t darken(uint16_t color);
    uint16_t darken(uint16_t color, float factor);
    void drawTremorRipple(uint8_t xStart, uint8_t yStart, const uint16_t* bg);

    bool eventSilencesText() const;

    uint16_t blend565(uint16_t bg, uint16_t fg, uint8_t alpha);
    void drawPhraseBlended(const char* phrase, uint16_t textColor, uint8_t alpha);
    void drawCharBlended(char c, int x, int y, uint16_t color, uint8_t alpha);

public:
    Clockface(Adafruit_GFX* display);
    ~Clockface();
    void setup(CWDateTime *dateTime);
    void update();
    void externalEvent(int type);

    static uint16_t framebuffer[64 * 64];    
};
}  // namespace dune
