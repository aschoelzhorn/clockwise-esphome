#pragma once

#include "IClockface.h"
#include "dune_IStoryTheme.h"
#include "CWDateTime.h"
#include "GFXWrapper.h"
#include "dune_act.h"

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
  IStoryTheme* theme_;
  GFXWrapper* gfx_;
  uint8_t currentActId_;
  uint32_t frameCount_;
  const char* currentPhrase_;
  CWDateTime* dt_;

  static constexpr uint32_t PHRASE_UPDATE_FRAMES = 300;  // Update phrase ~every 5 seconds at 60 FPS

public:
  StoryClockface(IStoryTheme* theme, GFXWrapper* gfx);
  ~StoryClockface();

  void setup(CWDateTime* dt) override;
  void update() override;
  void render(GFXWrapper* gfx, CWDateTime& dt);

private:
  void updateAct(CWDateTime& dt);
  void updatePhrase();
  void renderFrame(GFXWrapper* gfx, CWDateTime& dt);
};

}  // namespace dune
