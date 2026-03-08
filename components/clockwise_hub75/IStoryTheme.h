#pragma once

#include <stdint.h>
#include "CWDateTime.h"
#include "GFXWrapper.h"

namespace dune {
  class Act;
}

/**
 * IStoryTheme - Interface for story-based clockfaces
 * 
 * All story-based themes (Dune, Star Trek, Wednesday, etc.) implement this interface.
 * The generic StoryClockface orchestrator calls these methods to render theme-specific content.
 */
class IStoryTheme {
public:
  virtual ~IStoryTheme() = default;

  // ==================== Configuration ====================
  
  /**
   * Returns the number of acts in this theme's story
   * E.g., Dune has 6 acts, Star Trek might have 4
   */
  virtual uint8_t getActCount() const = 0;

  /**
   * Returns the duration of each act in seconds
   * E.g., Dune uses 4 hours (14400 seconds)
   */
  virtual uint32_t getActDurationSeconds() const = 0;

  /**
   * Human-readable theme name
   */
  virtual const char* getThemeName() const = 0;

  // ==================== Act Management ====================

  /**
   * Returns the Act object for the given act ID
   * @param actId Act index (0 to getActCount()-1)
   */
  virtual dune::Act* getAct(uint8_t actId) = 0;

  /**
   * Determines which act should be displayed for the current time
   * Default implementation: currentHour / (24 / getActCount())
   */
  virtual uint8_t getCurrentActId(CWDateTime& dt) const;

  // ==================== Rendering ====================

  /**
   * Render Layer 1: Static background image for this act
   */
  virtual void renderBackground(
    GFXWrapper* gfx,
    uint8_t actId
  ) = 0;

  /**
   * Render Layer 2: Ambient motion effects (sand, shimmer, shadows, etc.)
   * @param gfx Graphics wrapper
   * @param actId Current act ID
   * @param frameCount Frame counter (for deterministic animation)
   */
  virtual void renderAmbientEffect(
    GFXWrapper* gfx,
    uint8_t actId,
    uint32_t frameCount
  ) = 0;

  /**
   * Render Layer 4: Time display (HH:MM)
   * Theme controls positioning and styling
   */
  virtual void renderTimeDisplay(
    GFXWrapper* gfx,
    uint8_t actId,
    CWDateTime& dt
  ) = 0;

  /**
   * Render Layer 5: Text overlay (phrase)
   * Theme controls positioning and styling
   */
  virtual void renderTextField(
    GFXWrapper* gfx,
    uint8_t actId,
    const char* text
  ) = 0;
};
