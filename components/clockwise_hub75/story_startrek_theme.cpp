#include "story_startrek_theme.h"
#include "story_font.h"
#include "esphome/core/log.h"

static const char *const TAG = "StarTrekTheme";

namespace story {



// ==================== Act Initialization ====================

StarTrekTheme::StarTrekTheme() {
  // Initialize 4 acts with their respective data
  // Each act contains: id, name, phrase pool, font color, background image
  
  acts_[0] = Act(0, "Dawn of Exploration",
                 PHRASES_EXPLORATION,
                 STARFLEET_GOLD,
                 (const uint16_t*)startrek_act1_enterprise,
                 7);  // 7 seconds for testing

  acts_[1] = Act(1, "Starfleet Pride",
                 PHRASES_STARFLEET,
                 FEDERATION_BLUE,
                 (const uint16_t*)startrek_act2_starfleet,
                 7);

  acts_[2] = Act(2, "The Challenge",
                 PHRASES_CHALLENGE,
                 BORG_GREEN,
                 (const uint16_t*)startrek_act3_borg,
                 7);

  acts_[3] = Act(3, "Unity & Peace",
                 PHRASES_UNITY,
                 VULCAN_COPPER,
                 (const uint16_t*)startrek_act4_vulcan,
                 7);

  // Calculate total story duration for logging
  uint32_t totalDuration = 0;
  for (int i = 0; i < 4; i++) {
    totalDuration += acts_[i].getDurationSeconds();
  }
  ESP_LOGI(TAG, "Star Trek theme initialized: 4 acts, total story duration: %u seconds", totalDuration);
}

StarTrekTheme::~StarTrekTheme() {
  // Acts are stack-allocated, no cleanup needed
}

Act StarTrekTheme::getCurrentAct(uint8_t hour) {
  // 4 acts, each 6 hours
  uint8_t idx = hour / 6;  // 0-3
  if (idx >= getActCount()) {
    idx = getActCount() - 1;
  }
  return acts_[idx];
}

Act* StarTrekTheme::getAct(uint8_t actId) {
  if (actId < getActCount()) {
    return &acts_[actId];
  }
  return nullptr;
}

}  // namespace story

