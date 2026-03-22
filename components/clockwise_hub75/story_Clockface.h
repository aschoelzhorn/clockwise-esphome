#pragma once

#include "IClockface.h"
#include "story_IStoryTheme.h"
#include "CWDateTime.h"
#include "GFXWrapper.h"
#include "story_act.h"
#include "story_renderer.h"
#include "sprite_test_ship.h"

namespace story {

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
  StoryRenderer _renderer;
  uint8_t _activeActIndex = 0;
  Act _activeAct;

  CWDateTime* _dateTime;
  uint32_t _storyStartMs = 0;  // When the story cycle started
  uint32_t _now;
  uint32_t _lastMinuteChange = 0;
  uint8_t _lastMinute = 255;  // Invalid initial value

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

  static constexpr uint32_t PHRASE_UPDATE_FRAMES = 300;  // Update phrase ~every 5 seconds at 60 FPS

  TextState _text;

  // Test sprite/event
  TestShip _testShip;

public:
  StoryClockface(IStoryTheme* theme, Adafruit_GFX* gfx);
  ~StoryClockface();
  void setup(CWDateTime* dt) override;
  void update() override;
  void externalEvent(int type);

private:
  void setActiveAct();
  void enterAct(uint8_t actId);
  StoryTextOverlay updateTextOverlay();
};

}  // namespace story
