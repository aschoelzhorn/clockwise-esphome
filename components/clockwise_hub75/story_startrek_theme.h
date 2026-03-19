#pragma once

#include "story_IStoryTheme.h"
#include "story_act.h"
#include "story_startrek_phrases.h"
#include "story_startrek_assets.h"

namespace story {

/**
 * StarTrekTheme - Star Trek-specific implementation of IStoryTheme
 * 
 * Lightweight theme with:
 * - 4 Acts (Exploration, Starfleet, Challenge, Unity)
 * - 6-hour act duration (day divided into 4 acts)
 * - Star Trek narrative phrases
 * - Iconic backgrounds: Enterprise, Borg Cube, Starfleet Delta, Vulcan Salute
 */
class StarTrekTheme : public IStoryTheme {
private:
  Act acts_[4];
  // RGB565 color constants for Star Trek theme
const unsigned short STARFLEET_GOLD = 0xFDA0;     // Federation uniform color
const unsigned short FEDERATION_BLUE = 0x0357;    // Starfleet command blue
const unsigned short BORG_GREEN = 0x4E8F;         // Borg/tech menacing green
const unsigned short VULCAN_COPPER = 0xA3A0;      // Logical, earthy tone
const unsigned short NEBULA_PURPLE = 0x901F;      // Deep space mystery
const unsigned short PHASER_BRIGHT = 0xFDE0;      // Bright warning color

public:
  StarTrekTheme();
  ~StarTrekTheme();

  // IStoryTheme implementation
  uint8_t getActCount() const override { return 4; }
  uint32_t getActDurationSeconds() const override { return 6 * 3600; }
  const char* getThemeName() const override { return "StarTrek"; }
  Act* getAct(uint8_t actId) override;
  Act getCurrentAct(uint8_t hour);

};

}  // namespace story

