#pragma once

#include "story_IStoryTheme.h"
#include "story_act.h"
#include "story_dune_phrases.h"
#include "story_dune_assets.h"

namespace dune {

/**
 * DuneTheme - Dune-specific implementation of IStoryTheme
 * 
 * Encapsulates all Dune-specific content and rendering:
 * - 6 Acts (Desert Sleeps, Spice Awakens, The Watchers, etc.)
 * - 4-hour act duration
 * - Dune narrative phrases
 * - Sandy backgrounds and colors
 * - Ambient effects (heat shimmer, spice motes, shadows, etc.)
 */
class DuneTheme : public IStoryTheme {
private:
  Act acts_[6];
  
  // Rendering constants
  static constexpr int TEXT_Y = 2;
  static constexpr int FONT_W = 5;
  static constexpr int FONT_H = 7;
  static constexpr int CHAR_SPACING = 1;

  // Text animation
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

  TextState textState_;

  static constexpr uint32_t TEXT_FADE_MS = 1000;
  static constexpr uint32_t TEXT_HOLD_MS = 3000;
  static constexpr uint32_t TEXT_QUIET_MS = 1000;


  // Ambient effect states
  struct AmbientHeatState {
    bool enabled = false;
    uint32_t lastUpdate = 0;
    float phase = 0.0f;
  };
  AmbientHeatState ambientHeat_;

  struct AmbientShadowState {
    uint8_t shadowX = 0;
    uint8_t shadowY = 10;
    int8_t shadowDx = 1;
    uint32_t lastUpdate = 0;
  };
  AmbientShadowState ambientShadow_;

  struct AmbientTremorState {
    uint32_t lastUpdate = 0;
    static constexpr uint32_t UPDATE_MS = 100;
  };
  AmbientTremorState ambientTremor_;

 public:
  DuneTheme();
  ~DuneTheme();

  // IStoryTheme implementation
  uint8_t getActCount() const override { return 6; }  
  uint32_t getActDurationSeconds() const override { return 4 * 3600; }
  const char* getThemeName() const override { return "Dune"; }
  Act* getAct(uint8_t actId) override;
  Act getCurrentAct(uint8_t hour);
    
};

}  // namespace dune
