#pragma once

#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>

namespace story {

static constexpr size_t STORY_EVENT_MAX_SPRITES = 4;
static constexpr size_t STORY_ACT_MAX_EVENTS = 4;

struct StoryRenderSprite {
  const uint16_t* spriteData = nullptr;
  int16_t x = 0;
  int16_t y = 0;
  uint8_t width = 0;
  uint8_t height = 0;
};

struct StorySpriteMotion {
  const uint16_t* spriteData = nullptr;
  uint8_t width = 0;
  uint8_t height = 0;
  int16_t fromX = 0;
  int16_t fromY = 0;
  int16_t toX = 0;
  int16_t toY = 0;
  uint32_t startOffsetMs = 0;
  uint32_t durationMs = 1000;
};

enum class StoryEventTriggerType : uint8_t {
  RANDOM_INTERVAL = 0,
  ACT_TIME = 1,
};

struct StoryEventTrigger {
  StoryEventTriggerType type = StoryEventTriggerType::RANDOM_INTERVAL;
  uint32_t actTimeMs = 0;
  uint32_t minIntervalMs = 2500;
  uint32_t maxIntervalMs = 6000;
  bool repeat = true;
};

class StoryEvent {
 public:
  StoryEvent() = default;

  void configureRandom(uint32_t minIntervalMs, uint32_t maxIntervalMs, bool repeat = true) {
    trigger_.type = StoryEventTriggerType::RANDOM_INTERVAL;
    trigger_.minIntervalMs = minIntervalMs;
    trigger_.maxIntervalMs = maxIntervalMs;
    trigger_.repeat = repeat;
  }

  void configureActTime(uint32_t actTimeMs) {
    trigger_.type = StoryEventTriggerType::ACT_TIME;
    trigger_.actTimeMs = actTimeMs;
    trigger_.repeat = false;
  }

  bool addMotion(const StorySpriteMotion& motion) {
    if (motionCount_ >= STORY_EVENT_MAX_SPRITES) {
      return false;
    }
    motions_[motionCount_++] = motion;
    return true;
  }

  void reset(uint32_t actStartMs) {
    actStartMs_ = actStartMs;
    eventStartMs_ = 0;
    nextTriggerMs_ = 0;
    active_ = false;
    completed_ = false;

    if (trigger_.type == StoryEventTriggerType::RANDOM_INTERVAL) {
      nextTriggerMs_ = actStartMs_ + randomDelay_();
    }
  }

  void update(uint32_t now) {
    if (motionCount_ == 0 || completed_) {
      return;
    }

    if (active_) {
      if (now - eventStartMs_ >= totalDurationMs_()) {
        active_ = false;
        if (trigger_.type == StoryEventTriggerType::RANDOM_INTERVAL && trigger_.repeat) {
          nextTriggerMs_ = now + randomDelay_();
        } else {
          completed_ = true;
        }
      }
      return;
    }

    if (trigger_.type == StoryEventTriggerType::RANDOM_INTERVAL) {
      if (now >= nextTriggerMs_) {
        active_ = true;
        eventStartMs_ = now;
      }
      return;
    }

    uint32_t actElapsedMs = now - actStartMs_;
    if (actElapsedMs >= trigger_.actTimeMs) {
      active_ = true;
      eventStartMs_ = now;
    }
  }

  size_t collectVisibleSprites(uint32_t now, StoryRenderSprite* out, size_t maxSprites) const {
    if (!active_ || !out || maxSprites == 0) {
      return 0;
    }

    size_t count = 0;
    uint32_t eventElapsedMs = now - eventStartMs_;
    for (size_t i = 0; i < motionCount_ && count < maxSprites; i++) {
      const StorySpriteMotion& motion = motions_[i];
      if (motion.spriteData == nullptr || motion.durationMs == 0) {
        continue;
      }

      if (eventElapsedMs < motion.startOffsetMs) {
        continue;
      }

      uint32_t motionElapsedMs = eventElapsedMs - motion.startOffsetMs;
      if (motionElapsedMs > motion.durationMs) {
        continue;
      }

      StoryRenderSprite& renderSprite = out[count++];
      renderSprite.spriteData = motion.spriteData;
      renderSprite.width = motion.width;
      renderSprite.height = motion.height;
      renderSprite.x = lerp_(motion.fromX, motion.toX, motionElapsedMs, motion.durationMs);
      renderSprite.y = lerp_(motion.fromY, motion.toY, motionElapsedMs, motion.durationMs);
    }

    return count;
  }

 private:
  StorySpriteMotion motions_[STORY_EVENT_MAX_SPRITES];
  size_t motionCount_ = 0;
  StoryEventTrigger trigger_;
  uint32_t actStartMs_ = 0;
  uint32_t eventStartMs_ = 0;
  uint32_t nextTriggerMs_ = 0;
  bool active_ = false;
  bool completed_ = false;

  uint32_t randomDelay_() const {
    if (trigger_.maxIntervalMs <= trigger_.minIntervalMs) {
      return trigger_.minIntervalMs;
    }
    return random(trigger_.minIntervalMs, trigger_.maxIntervalMs + 1);
  }

  uint32_t totalDurationMs_() const {
    uint32_t total = 0;
    for (size_t i = 0; i < motionCount_; i++) {
      uint32_t endMs = motions_[i].startOffsetMs + motions_[i].durationMs;
      if (endMs > total) {
        total = endMs;
      }
    }
    return total;
  }

  static int16_t lerp_(int16_t from, int16_t to, uint32_t elapsedMs, uint32_t durationMs) {
    if (durationMs == 0 || elapsedMs >= durationMs) {
      return to;
    }

    int32_t delta = static_cast<int32_t>(to) - static_cast<int32_t>(from);
    return static_cast<int16_t>(from + ((delta * static_cast<int32_t>(elapsedMs)) / static_cast<int32_t>(durationMs)));
  }
};

}  // namespace story