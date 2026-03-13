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
  GFXWrapper* _display;
  uint8_t _currentActId;
  uint32_t _frameCount;
  const char* _currentPhrase;
  CWDateTime* _dateTime;
  uint32_t _now;

  // Background transition state
  struct BackgroundTransition {
    bool active = false;
    const uint16_t* from = nullptr;
    const uint16_t* to = nullptr;
    uint32_t start = 0;
    uint32_t duration = 2500;
  };
  BackgroundTransition _bgTransition;

  static constexpr uint32_t PHRASE_UPDATE_FRAMES = 300;  // Update phrase ~every 5 seconds at 60 FPS

public:
  StoryClockface(IStoryTheme* theme, GFXWrapper* gfx);
  ~StoryClockface();
  void setup(CWDateTime* dt) override;
  void update() override;
  void externalEvent(int type);

  static uint16_t framebuffer[64 * 64];    

private:
  void updateAct(CWDateTime& dt);
  void updatePhrase();
  void render(GFXWrapper* gfx, CWDateTime& dt);
  void renderFrame(GFXWrapper* gfx, CWDateTime& dt);
  void flushFramebuffer();
  
  // Framebuffer operations
  inline void fbClear(uint16_t color);
  inline uint16_t fbGet(uint8_t x, uint8_t y);
  inline void fbSet(uint8_t x, uint8_t y, uint16_t color);
  
  // Background rendering with transition
  void renderBackgroundLayer();
  uint16_t blend565(uint16_t bg, uint16_t fg, uint8_t alpha);
};

}  // namespace dune
