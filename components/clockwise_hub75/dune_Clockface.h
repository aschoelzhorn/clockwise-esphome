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

typedef struct {
  uint32_t now_ms;

  uint8_t act_index;              // 0..5
  uint8_t hour, minute;
  bool colon_on;

  bool text_active;
  bool event_active;

} RenderContext;

typedef enum {
  VS_IDLE,
  VS_ENTER,
  VS_ACTIVE,
  VS_EXIT
} VisualPhase;

typedef struct {
  bool active;
  VisualPhase phase;
  uint32_t phase_start_ms;
} VisualEvent;

typedef struct {
  bool active;
  EventType type;

  VisualPhase phase;
  uint32_t phase_start_ms;
} EventState;

EventState event = {0};

#define STORM_ENTER_MS   800
#define STORM_ACTIVE_MS 3000
#define STORM_EXIT_MS   1200


typedef enum {
  EVENT_NONE = 0,
  EVENT_STORM,
  EVENT_WORM,
  EVENT_FLIGHT
} EventType;


    Adafruit_GFX* _display;
    CWDateTime* _dateTime;
    Act _act;
    Act acts[6];

    uint32_t _lastPhraseUpdate = 0;

    EventBus* _eventBus;

    void drawTime(uint8_t hour, uint8_t minute, uint16_t color);
    void drawDigit(uint8_t digit, int x, int y, uint16_t color);
    void drawColon(int x, int y, bool blink, uint16_t color);      
    Act getCurrentAct(uint8_t hour);
    void updateTime();
    void printPhrase(const char* phrase);
    void initializeActs();
    void drawBackgroundImage(const uint16_t* image);

    void layer_clear(const RenderContext* ctx);
    void layer_background(const RenderContext* ctx);
    void layer_ambient(const RenderContext* ctx);
    void layer_event(const RenderContext* ctx);
    void layer_time(const RenderContext* ctx);
    void layer_text(const RenderContext* ctx);


public:
    Clockface(Adafruit_GFX* display);
    ~Clockface();
    void setup(CWDateTime *dateTime);
    void update();
    void externalEvent(int type);
};
}  // namespace dune
