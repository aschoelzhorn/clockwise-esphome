#pragma once

#include <stdint.h>
#include "CWDateTime.h"
#include "GFXWrapper.h"
#include "story_act.h"
#include "story_font.h"

namespace story {

using GlyphFont = const uint8_t (*)[5];

/**
 * IStoryTheme - Interface for story-based clockfaces
 * 
 * All story-based themes (Dune, Star Trek, Wednesday, etc.) implement this interface.
 * The generic StoryClockface orchestrator calls these methods to render theme-specific content.
 */
class IStoryTheme {
public:
  virtual ~IStoryTheme() = default;

  /**
   * Human-readable theme name
   */
  virtual const char* getThemeName() const = 0;

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
   * Returns the Act object for the given act ID
   * @param actId Act index (0 to getActCount()-1)
   */
  virtual Act* getAct(uint8_t actId) = 0;

  /**
   * Determines which act should be displayed for the current time
   * Default implementation: currentHour / (24 / getActCount())
   */
  virtual uint8_t getCurrentActId(CWDateTime& dt) const;

  /**
   * Returns the glyph table used for phrase rendering.
   * Default: dune_font5x7. Override in theme to use a different 5x7 font.
   */
  virtual GlyphFont getPhraseFont() const { return dune_font5x7; }

};

}  // namespace story
