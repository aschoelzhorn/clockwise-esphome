#include "StoryClockface.h"
#include "Act.h"
#include "esphome/core/log.h"

static const char *const TAG = "StoryClockface";

StoryClockface::StoryClockface(IStoryTheme* theme, GFXWrapper* gfx)
  : theme_(theme), gfx_(gfx), currentActId_(0), frameCount_(0),
    currentPhrase_(nullptr), dt_(nullptr) {
  if (!theme_) {
    ESP_LOGE(TAG, "Theme cannot be null");
  }
}

StoryClockface::~StoryClockface() {
  // Don't delete theme_ - caller owns it
}

void StoryClockface::setup(CWDateTime* dt) {
  dt_ = dt;
  if (!dt_) {
    ESP_LOGE(TAG, "CWDateTime pointer cannot be null");
    return;
  }
  
  // Initialize with current act
  currentActId_ = theme_->getCurrentActId(*dt_);
  Act* act = theme_->getAct(currentActId_);
  if (act) {
    currentPhrase_ = act->getNewPhrase();
    ESP_LOGI(TAG, "StoryClockface initialized - Theme: %s, Act: %s",
             theme_->getThemeName(), act->getName());
  }
}

void StoryClockface::render(GFXWrapper* gfx, const CWDateTime& dt) {
  if (!theme_ || !gfx) {
    return;
  }

  updateAct(dt);
  
  // Update phrase occasionally
  if (frameCount_ % PHRASE_UPDATE_FRAMES == 0) {
    updatePhrase();
  }

  renderFrame(gfx, dt);
  frameCount_++;
}

void StoryClockface::updateAct(const CWDateTime& dt) {
  uint8_t newActId = theme_->getCurrentActId(dt);
  
  if (newActId != currentActId_) {
    currentActId_ = newActId;
    Act* act = theme_->getAct(currentActId_);
    if (act) {
      ESP_LOGI(TAG, "Act changed: %s", act->getName());
      // Get first phrase for new act
      currentPhrase_ = act->getNewPhrase();
    }
  }
}

void StoryClockface::updatePhrase() {
  Act* act = theme_->getAct(currentActId_);
  if (act) {
    currentPhrase_ = act->getNewPhrase();
  }
}

void StoryClockface::renderFrame(GFXWrapper* gfx, const CWDateTime& dt) {
  // Layer 0: Clear
  gfx->fillScreen(0x0000);  // Black

  // Layer 1: Background (theme-specific)
  theme_->renderBackground(gfx, currentActId_);

  // Layer 2: Ambient effects (theme-specific)
  theme_->renderAmbientEffect(gfx, currentActId_, frameCount_);

  // Layer 3: Major events (placeholder for future)
  // theme_->renderMajorEvents(gfx, currentActId_, frameCount_);

  // Layer 4: Time display (theme-specific)
  theme_->renderTimeDisplay(gfx, currentActId_, dt);

  // Layer 5: Text overlay (theme-specific)
  if (currentPhrase_) {
    theme_->renderTextField(gfx, currentActId_, currentPhrase_);
  }
}
