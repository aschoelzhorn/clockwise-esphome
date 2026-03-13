#pragma once

#include "dune_IStoryTheme.h"
#include "dune_act.h"
#include "dune_phrases.h"
#include "dune_assets.h"

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
  
  // Design note: These rendering constants are theme-specific (Dune-specific)
  // Future refactor: Could extract common rendering utilities to a base class
  // if multiple themes share similar text/rendering patterns

  uint32_t frameCount_;

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

  // Design note: Helper methods kept in theme for encapsulation
  // Color blending and text rendering are theme-specific in this design
  // Future refactor: Could extract to shared utilities if patterns emerge

  // Helper methods for color blending
  static uint16_t blend565(uint16_t bg, uint16_t fg, uint8_t alpha);
  static uint16_t darken(uint16_t color, float factor = 0.75f);
  static inline uint8_t r5(uint16_t c) { return (c >> 11) & 0x1F; }
  static inline uint8_t g6(uint16_t c) { return (c >> 5) & 0x3F; }
  static inline uint8_t b5(uint16_t c) { return c & 0x1F; }
  static inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 11) | (g << 5) | b;
  }

  // Helper methods
  int textWidth(const char* s) const;
  void renderTextPhrase(GFXWrapper* gfx, const char* phrase, uint16_t color);
  void drawCharBlended(GFXWrapper* gfx, char c, int x, int y, uint16_t color, uint8_t alpha);
  void drawPhraseBlended(GFXWrapper* gfx, const char* phrase, uint16_t color, uint8_t alpha);
  void updateTextState(uint32_t frameTime);

  // Ambient effect implementations
  void renderAmbientEffect_I(GFXWrapper* gfx, uint32_t frameCount);
  void renderAmbientEffect_II(GFXWrapper* gfx, uint32_t frameCount);
  void renderAmbientEffect_III(GFXWrapper* gfx, uint32_t frameCount);
  void renderAmbientEffect_IV(GFXWrapper* gfx, uint32_t frameCount);
  void renderAmbientEffect_V(GFXWrapper* gfx, uint32_t frameCount);
  void renderAmbientEffect_VI(GFXWrapper* gfx, uint32_t frameCount);

  // Specific ambient effect helpers
  void drawShadowBand(GFXWrapper* gfx, uint8_t xStart, uint8_t yStart, const uint16_t* bg);
  void drawTremorRipple(GFXWrapper* gfx, uint8_t x, uint8_t y, const uint16_t* bg);

public:
  DuneTheme();
  ~DuneTheme();

  // IStoryTheme implementation
  uint8_t getActCount() const override { return 6; }  
  uint32_t getActDurationSeconds() const override { return 4 * 3600; }
  const char* getThemeName() const override { return "Dune"; }

  Act* getAct(uint8_t actId) override;
  
  // Design note: Theme-specific rendering implementation (IStoryTheme interface)
  // Themes handle their own rendering logic to allow full customization per theme
  void renderBackground(GFXWrapper* gfx, uint8_t actId) override;
  void renderAmbientEffect(GFXWrapper* gfx, uint8_t actId, uint32_t frameCount) override;
  void renderTimeDisplay(GFXWrapper* gfx, uint8_t actId, CWDateTime& dt) override;
  void renderTextField(GFXWrapper* gfx, uint8_t actId, const char* text) override;
};

}  // namespace dune
