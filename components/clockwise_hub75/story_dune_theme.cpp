#include "story_dune_theme.h"
#include "story_font.h"
#include "mario_assets.h"
#include "esphome/core/log.h"
#include <cmath>
#include <cstring>

static const char *const TAG = "DuneTheme";

namespace story {



// ==================== Act Initialization ====================

DuneTheme::DuneTheme() {
  // Initialize 6 acts with their respective data
  // Each act contains: id, name, phrase pool, font color, background image
  
  acts_[0] = Act(0, "The Desert Sleeps",
                 PHRASES_TIME,
                 DIM_SAND,
                 dune_act1,
                 7);  // 7 seconds for testing

  acts_[1] = Act(1, "Spice Awakens",
                 PHRASES_DESERT,
                 SPICE_AMBER,
                 dune_act2,
                 7);

  acts_[2] = Act(2, "The Watchers",
                 PHRASES_POWER,
                 HIGH_CONTRAST_WHITE,
                 dune_act3,
                 7);

  acts_[3] = Act(3, "The Maker Stirs",
                 PHRASES_DANGER,
                 BRIGHT_SAND,
                 dune_act4,
                 7);

  acts_[4] = Act(4, "Storm of Fate",
                 PHRASES_DANGER,
                 RED_DANGER,
                 dune_act5,
                 7);

  acts_[5] = Act(5, "Silence & Survival",
                 PHRASES_SURVIVAL,
                 COOL_BROWN,
                 dune_act6,
                 7);  // 7 seconds for testing

  StoryEvent testEvent;
  testEvent.configureRandom(900, 1800, true);
  testEvent.addMotion({GOOMBA, ENEMY_SIZE[0], ENEMY_SIZE[1], -8, 18, 64, 18, 0, 1800});
  testEvent.addMotion({KOOPA, ENEMY_SIZE[0], ENEMY_SIZE[1], 64, 28, -8, 28, 300, 2200});
  acts_[0].addEvent(testEvent);

  // Calculate total story duration for logging
  uint32_t totalDuration = 0;
  for (int i = 0; i < 6; i++) {
    totalDuration += acts_[i].getDurationSeconds();
  }
  ESP_LOGI(TAG, "Dune theme initialized: 6 acts, total story duration: %u seconds", totalDuration);

  // // Initialize ambient states
  // ambientHeat_.enabled = false;
  // ambientHeat_.lastUpdate = 0;
  // ambientHeat_.phase = 0.0f;

  // ambientShadow_.shadowX = 0;
  // ambientShadow_.shadowY = 10;
  // ambientShadow_.shadowDx = 1;
  // ambientShadow_.lastUpdate = 0;

  // ambientTremor_.lastUpdate = 0;
}

DuneTheme::~DuneTheme() {
  // Acts are stack-allocated, no cleanup needed
}

Act DuneTheme::getCurrentAct(uint8_t hour) {

#ifdef TEST_ACT_CYCLING
    // Change act every 10 seconds
    static constexpr uint32_t TEST_ACT_MS = 10 * 1000;
    uint32_t slot = (millis() / TEST_ACT_MS) % 6;
    return acts_[slot];
#else
	// 6 phases, each 4 hours
    uint8_t idx = hour / 4; // 0-5
    return acts_[idx];
#endif
}


Act* DuneTheme::getAct(uint8_t actId) {
  if (actId < getActCount()) {
    return &acts_[actId];
  }
  return nullptr;
}


}  // namespace story
