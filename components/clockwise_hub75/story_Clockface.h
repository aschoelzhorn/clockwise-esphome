#pragma once

#include "IClockface.h"
#include "story_IStoryTheme.h"
#include "CWDateTime.h"
#include "GFXWrapper.h"
#include "story_act.h"

namespace dune {

/**
 * StoryClockface - Generic orchestrator for story-based clockfaces
 * 
 * This class handles the universal rendering pipeline for any story theme:
 * L0: Clear
 * L1: Background (theme-specific)
 * L2: Ambient effects (theme-specific)
 * L3: Major events (placeholder)
 * L4: Time display (theme-specific)
 * L5: Text overlay (theme-specific)
 * 
 * Themes implement IStoryTheme to customize their content and rendering.
 */
class StoryClockface : public IClockface {
private:
  IStoryTheme* _theme;
  Adafruit_GFX* _display;
  //uint8_t _currentActId;
  //uint32_t _frameCount;
  const char* _currentPhrase;

   Act _activeAct;

  CWDateTime* _dateTime;
  uint32_t _now;
  uint32_t _lastMinuteChange = 0;
  uint8_t _lastMinute = 255;  // Invalid initial value


  // Background transition state
  struct BackgroundTransition {
    bool active = false;
    const uint16_t* from = nullptr;
    const uint16_t* to = nullptr;
    uint32_t start = 0;
    uint32_t duration = 2500;
  };
  BackgroundTransition _bgTransition;


    #define ACT_I 1
    #define ACT_II 2
    #define ACT_III 3
    #define ACT_IV 4
    #define ACT_V 5
    #define ACT_VI 6


  // Text rendering constants
  static constexpr int TEXT_Y = 2;
  static constexpr int FONT_W = 5;
  static constexpr int FONT_H = 7;
  static constexpr int CHAR_SPACING = 1;

  // Text state machine
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

  static constexpr uint32_t TEXT_FADE_MS = 1000;
  static constexpr uint32_t TEXT_HOLD_MS = 3000;
  static constexpr uint32_t TEXT_QUIET_MS = 1500;
  static constexpr uint32_t PHRASE_UPDATE_FRAMES = 300;  // Update phrase ~every 5 seconds at 60 FPS

  TextState _text;

public:
  StoryClockface(IStoryTheme* theme, Adafruit_GFX* gfx);
  ~StoryClockface();
  void setup(CWDateTime* dt) override;
  void update() override;
  void externalEvent(int type);

  static uint16_t framebuffer[64 * 64];    

private:
  // void updateAct(CWDateTime& dt);
  void updatePhrase();
  void render(Adafruit_GFX* gfx);
  void flushFramebuffer();
  
  void setActiveAct();
  void enterAct(uint8_t actId);

  // void drawBackgroundImage(const uint16_t* image);
  void drawTime(uint8_t hour, uint8_t minute, uint16_t color);
  // void drawDigit(uint8_t digit, int x, int y, uint16_t color);
  // void drawColon(int x, int y, bool blink, uint16_t color);

  // rendering layers
  void layer_clear();
  void layer_background();
  void layer_ambient();
  void layer_event();
  void layer_time();
  void layer_text();

  // event effects
  // void maybeStartEvent();
  // void startEvent(EventType type);
  // void endEvent();

  // Framebuffer operations
  inline void fbClear(uint16_t color);
  inline uint16_t fbGet(uint8_t x, uint8_t y);
  inline void fbSet(uint8_t x, uint8_t y, uint16_t color);
  
  // blending rendering with transition
  uint16_t blend565(uint16_t bg, uint16_t fg, uint8_t alpha);
  void drawPhraseBlended(const char* phrase, uint16_t textColor, uint8_t alpha);
  void drawCharBlended(char c, int x, int y, uint16_t color, uint8_t alpha);

  int textWidth(const char* s);
};

}  // namespace dune
