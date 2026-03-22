#pragma once

#include <Adafruit_GFX.h>
#include <stddef.h>
#include "CWDateTime.h"
#include "story_IStoryTheme.h"
#include "story_act.h"
#include "story_event.h"

namespace story {

struct StoryTextOverlay {
  const char* phrase = nullptr;
  uint16_t color = 0;
  uint8_t alpha = 0;
  bool visible = false;
};

class StoryRenderer {
public:
  explicit StoryRenderer(Adafruit_GFX* display);

  void render(const IStoryTheme& theme, const Act& activeAct, const CWDateTime& dateTime,
              uint32_t now, const StoryTextOverlay& overlay,
              const StoryRenderSprite* eventSprites, size_t eventSpriteCount);
  void startActTransition(const Act& fromAct, const Act& toAct, uint32_t now);
  void clearTransition();

private:
  struct BackgroundTransition {
    bool active = false;
    const uint16_t* from = nullptr;
    const uint16_t* to = nullptr;
    uint32_t start = 0;
    uint32_t duration = 2500;
  };

  static constexpr int FONT_W = 5;
  static constexpr int FONT_H = 7;
  static constexpr int CHAR_SPACING = 1;

  Adafruit_GFX* display_;
  uint16_t framebuffer_[64 * 64];
  BackgroundTransition bg_transition_;

  void flushFramebuffer();
  void layerClear();
  void layerBackground(const Act& activeAct, uint32_t now);
  void layerAmbient();
  void layerEvent(const StoryRenderSprite* eventSprites, size_t eventSpriteCount);
  void layerTime(const Act& activeAct, const CWDateTime& dateTime, uint32_t now);
  void layerText(const IStoryTheme& theme, const Act& activeAct, const StoryTextOverlay& overlay);

  inline void fbClear(uint16_t color);
  inline uint16_t fbGet(uint8_t x, uint8_t y) const;
  inline void fbSet(uint8_t x, uint8_t y, uint16_t color);

  uint16_t blend565(uint16_t bg, uint16_t fg, uint8_t alpha) const;
  void drawPhraseBlended(const IStoryTheme& theme, const Act& activeAct, const char* phrase,
                         uint16_t textColor, uint8_t alpha);
  void drawCharBlended(const IStoryTheme& theme, char c, int x, int y, uint16_t color,
                       uint8_t alpha);
  int textWidth(const char* s) const;
  void drawTime(const Act& activeAct, uint8_t hour, uint8_t minute, uint32_t now, uint16_t color);
};

}  // namespace story