#include "story_Clockface.h"
#include "story_act.h"
#include "esphome/core/log.h"
#include <Adafruit_GFX.h>

// Enable test mode to cycle acts every 15 seconds
// #define TEST_ACT_CYCLING

namespace story {

static const char *const TAG = "StoryClockface";

StoryClockface::StoryClockface(IStoryTheme* theme, Adafruit_GFX* gfx) : _renderer(gfx) {
  if (!theme) {
    ESP_LOGE(TAG, "Theme cannot be null");
  }
  if (!gfx) {
    ESP_LOGE(TAG, "Adafruit_GFX cannot be null");
  }  
  _theme = theme;
  _activeActIndex = 0;
  _activeAct = nullptr;
  _dateTime = nullptr;
  _storyStartMs = 0;
  _activeActStartMs = 0;
  _now = 0;
}

StoryClockface::~StoryClockface() {
  // Don't delete theme_ - caller owns it
    // delete _eventBus;
}

void StoryClockface::setup(CWDateTime* dt) {
  _dateTime = dt;
  if (!_dateTime) {
    ESP_LOGE(TAG, "CWDateTime pointer cannot be null");
    return;
  }
  
  // Start the story cycle now
  _storyStartMs = millis();
  _activeActStartMs = _storyStartMs;
  _activeActIndex = 0;
  Act* firstAct = _theme ? _theme->getAct(0) : nullptr;
  if (firstAct == nullptr) {
    ESP_LOGE(TAG, "Theme did not provide act 0");
    return;
  }

  _activeAct = firstAct;
  _activeAct->resetEvents(_activeActStartMs);
  _renderer.clearTransition();

  ESP_LOGI(TAG, "StoryClockface initialized - Theme: %s, starting at act 0 (duration: %u sec)",
           _theme->getThemeName(), firstAct ? firstAct->getDurationSeconds() : 0);
}

void StoryClockface::update() {
  _now = millis();

  if (!_theme || !_dateTime) {
    return;
  }
  
  // Track minute changes for event timing
  uint8_t currentMinute = _dateTime->getMinute();
  if (currentMinute != _lastMinute) {
      _lastMinute = currentMinute;
      _lastMinuteChange = _now;
  }

  setActiveAct(); // Update active Act based on current hour
  if (_activeAct != nullptr) {
    _activeAct->updateEvents(_now);
    _eventSpriteCount = _activeAct->collectRenderSprites(_now, _eventSprites,
                                                         STORY_ACT_MAX_EVENTS * STORY_EVENT_MAX_SPRITES);
  } else {
    _eventSpriteCount = 0;
  }

  StoryTextOverlay overlay = updateTextOverlay();
  if (_activeAct != nullptr) {
    _renderer.render(*_theme, *_activeAct, *_dateTime, _now, overlay, _eventSprites, _eventSpriteCount);
  }
}

void StoryClockface::setActiveAct() {
  const uint8_t actCount = _theme->getActCount();
  if (actCount == 0) {
    return;
  }

  uint32_t totalDurationSec = 0;
  for (uint8_t i = 0; i < actCount; i++) {
    Act* act = _theme->getAct(i);
    if (!act) continue;
    uint32_t duration = act->getDurationSeconds();
    totalDurationSec += (duration > 0) ? duration : 1;
  }
  if (totalDurationSec == 0) {
    return;
  }

  const uint32_t elapsedSec = (_now - _storyStartMs) / 1000;
  const uint32_t cyclePos = elapsedSec % totalDurationSec;

  uint32_t cumulative = 0;
  uint8_t newActIndex = 0;
  for (uint8_t i = 0; i < actCount; i++) {
    Act* act = _theme->getAct(i);
    if (!act) continue;
    uint32_t duration = act->getDurationSeconds();
    duration = (duration > 0) ? duration : 1;
    cumulative += duration;
    if (cyclePos < cumulative) {
      newActIndex = i;
      break;
    }
  }

  Act* currentAct = _theme->getAct(newActIndex);
  if (!currentAct) {
    return;
  }

  // Detect act change
  if (newActIndex != _activeActIndex) {
    ESP_LOGI(TAG, "Act transition: %d -> %d (elapsed: %u sec, cycle: %u/%u)",
             _activeActIndex, newActIndex, elapsedSec, cyclePos, totalDurationSec);
    enterAct(newActIndex);
  }

  _activeActIndex = newActIndex;
  _activeAct = currentAct;
}

StoryTextOverlay StoryClockface::updateTextOverlay() {
  StoryTextOverlay overlay;
  uint32_t elapsed = _now - _text.phaseStart;
  if (_activeAct == nullptr) {
    return overlay;
  }

  const ActLayout& layout = _activeAct->getLayout();

  switch (_text.phase) {
    case TEXT_IDLE:
      _text.phrase = _activeAct->getNewPhrase();
      if (!_text.phrase) return overlay;
      _text.phase = TEXT_FADE_IN;
      _text.phaseStart = _now;
      break;

    case TEXT_FADE_IN:
      overlay.visible = true;
      overlay.phrase = _text.phrase;
      overlay.color = _activeAct->getFontColor();
      overlay.alpha = ((elapsed * 255) / layout.textFadeMs > 255) ? 255 : (elapsed * 255) / layout.textFadeMs;
      if (elapsed >= layout.textFadeMs) {
        _text.phase = TEXT_HOLD;
        _text.phaseStart = _now;
      }
      break;

    case TEXT_HOLD:
      overlay.visible = true;
      overlay.phrase = _text.phrase;
      overlay.color = _activeAct->getFontColor();
      overlay.alpha = 255;
      if (_now - _text.phaseStart >= layout.textHoldMs) {
        _text.phase = TEXT_FADE_OUT;
        _text.phaseStart = _now;
      }
      break;

    case TEXT_FADE_OUT:
      overlay.visible = true;
      overlay.phrase = _text.phrase;
      overlay.color = _activeAct->getFontColor();
      overlay.alpha = 255 - ((((elapsed * 255) / layout.textFadeMs) > 255) ? 255 : ((elapsed * 255) / layout.textFadeMs));
      if (elapsed >= layout.textFadeMs) {
        _text.phase = TEXT_QUIET;
        _text.phaseStart = _now;
      }
      break;

    case TEXT_QUIET:
      if (_now - _text.phaseStart >= layout.textQuietMs) {
        _text.phase = TEXT_IDLE;
      }
      break;
  }

  return overlay;
}

void StoryClockface::enterAct(uint8_t actIndex) {
  Act* newAct = _theme->getAct(actIndex);

  if (!newAct) {
    return;
  }

  // No change to transition between
  if (_activeActIndex == actIndex) {
    _renderer.clearTransition();
    return;
  }

  // Transition from current active act to target act
  if (_activeAct != nullptr) {
    _renderer.startActTransition(*_activeAct, *newAct, _now);
  } else {
    _renderer.clearTransition();
  }

  _activeActStartMs = _now;
  newAct->resetEvents(_activeActStartMs);
  _text.phase = TEXT_IDLE;
  _text.phrase = nullptr;
  _text.phaseStart = _now;
}

}  // namespace story
