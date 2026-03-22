#include "story_renderer.h"

#include "esphome/core/log.h"
#include "story_font.h"
#include "mario_assets.h"

#include <pgmspace.h>

namespace story {

namespace {

class FB_GFX : public Adafruit_GFX {
public:
  explicit FB_GFX(uint16_t* framebuffer) : Adafruit_GFX(64, 64), framebuffer_(framebuffer) {}

  void drawPixel(int16_t x, int16_t y, uint16_t color) override {
    if (x < 0 || y < 0 || x >= 64 || y >= 64) return;
    framebuffer_[y * 64 + x] = color;
  }

private:
  uint16_t* framebuffer_;
};

static const char *const TAG = "StoryRenderer";

}  // namespace

StoryRenderer::StoryRenderer(Adafruit_GFX* display) : display_(display), framebuffer_{0} {}

void StoryRenderer::render(const IStoryTheme& theme, const Act& activeAct, const CWDateTime& dateTime,
                           uint32_t now, const StoryTextOverlay& overlay,
                           const StoryRenderSprite* eventSprites, size_t eventSpriteCount) {
  if (!display_) {
    ESP_LOGE(TAG, "Display cannot be null");
    return;
  }

  layerClear();
  layerBackground(activeAct, now);
  layerAmbient();
  layerEvent(eventSprites, eventSpriteCount);
  layerTime(activeAct, dateTime, now);
  layerText(theme, activeAct, overlay);
  flushFramebuffer();
}

void StoryRenderer::startActTransition(const Act& fromAct, const Act& toAct, uint32_t now) {
  const uint16_t* oldBg = fromAct.getBackground();
  const uint16_t* newBg = toAct.getBackground();
  if (!oldBg || !newBg) {
    clearTransition();
    return;
  }

  bg_transition_.active = true;
  bg_transition_.from = oldBg;
  bg_transition_.to = newBg;
  bg_transition_.start = now;
}

void StoryRenderer::clearTransition() {
  bg_transition_.active = false;
  bg_transition_.from = nullptr;
  bg_transition_.to = nullptr;
  bg_transition_.start = 0;
}

void StoryRenderer::flushFramebuffer() {
  display_->drawRGBBitmap(0, 0, framebuffer_, 64, 64);
}

void StoryRenderer::layerClear() {
  fbClear(0x0000);
}

void StoryRenderer::layerBackground(const Act& activeAct, uint32_t now) {
  if (!bg_transition_.active) {
    const uint16_t* bg = activeAct.getBackground();
    if (!bg) {
      ESP_LOGE(TAG, "layerBackground() failed: bg is nullptr");
      return;
    }
    memcpy(framebuffer_, bg, 64 * 64 * sizeof(uint16_t));
    return;
  }

  uint32_t elapsed = now - bg_transition_.start;
  if (elapsed >= bg_transition_.duration) {
    bg_transition_.active = false;

    const uint16_t* bg = bg_transition_.to;
    for (uint8_t y = 0; y < 64; y++) {
      for (uint8_t x = 0; x < 64; x++) {
        fbSet(x, y, bg[y * 64 + x]);
      }
    }
    return;
  }

  uint8_t alpha = (elapsed * 255) / bg_transition_.duration;
  const uint16_t* a = bg_transition_.from;
  const uint16_t* b = bg_transition_.to;

  for (uint8_t y = 0; y < 64; y++) {
    for (uint8_t x = 0; x < 64; x++) {
      uint16_t blended = blend565(a[y * 64 + x], b[y * 64 + x], alpha);
      fbSet(x, y, blended);
    }
  }
}

void StoryRenderer::layerAmbient() {
  return;
}

void StoryRenderer::layerEvent(const StoryRenderSprite* eventSprites, size_t eventSpriteCount) {
  if (!eventSprites || eventSpriteCount == 0) {
    return;
  }

  for (size_t index = 0; index < eventSpriteCount; index++) {
    const StoryRenderSprite& sprite = eventSprites[index];
    if (!sprite.spriteData || sprite.width == 0 || sprite.height == 0) {
      continue;
    }

    for (uint8_t sy = 0; sy < sprite.height; sy++) {
      for (uint8_t sx = 0; sx < sprite.width; sx++) {
        int spriteIdx = sy * sprite.width + sx;
        uint16_t color = sprite.spriteData[spriteIdx];
        if (color == TRANSPARENT) {
          continue;
        }

        int px = sprite.x + sx;
        int py = sprite.y + sy;
        if (px >= 0 && px < 64 && py >= 0 && py < 64) {
          fbSet(px, py, color);
        }
      }
    }
  }
}

void StoryRenderer::layerTime(const Act& activeAct, const CWDateTime& dateTime, uint32_t now) {
  drawTime(activeAct, dateTime.getHour(), dateTime.getMinute(), now, activeAct.getFontColor());
}

void StoryRenderer::layerText(const IStoryTheme& theme, const Act& activeAct,
                              const StoryTextOverlay& overlay) {
  if (!overlay.visible || !overlay.phrase) {
    return;
  }

  drawPhraseBlended(theme, activeAct, overlay.phrase, overlay.color, overlay.alpha);
}

inline void StoryRenderer::fbClear(uint16_t color) {
  for (uint16_t i = 0; i < 64 * 64; i++) {
    framebuffer_[i] = color;
  }
}

inline uint16_t StoryRenderer::fbGet(uint8_t x, uint8_t y) const {
  return framebuffer_[y * 64 + x];
}

inline void StoryRenderer::fbSet(uint8_t x, uint8_t y, uint16_t color) {
  framebuffer_[y * 64 + x] = color;
}

uint16_t StoryRenderer::blend565(uint16_t bg, uint16_t fg, uint8_t alpha) const {
  uint8_t r1 = (bg >> 11) & 0x1F;
  uint8_t g1 = (bg >> 5) & 0x3F;
  uint8_t b1 = bg & 0x1F;

  uint8_t r2 = (fg >> 11) & 0x1F;
  uint8_t g2 = (fg >> 5) & 0x3F;
  uint8_t b2 = fg & 0x1F;

  uint8_t r = ((r1 * (255 - alpha)) + (r2 * alpha)) / 255;
  uint8_t g = ((g1 * (255 - alpha)) + (g2 * alpha)) / 255;
  uint8_t b = ((b1 * (255 - alpha)) + (b2 * alpha)) / 255;

  return (r << 11) | (g << 5) | b;
}

void StoryRenderer::drawPhraseBlended(const IStoryTheme& theme, const Act& activeAct,
                                      const char* phrase, uint16_t textColor, uint8_t alpha) {
  if (!phrase) return;

  int w = textWidth(phrase);
  int x = (64 - w) / 2;
  int y = activeAct.getLayout().phraseY;

  for (size_t i = 0; i < strlen(phrase); i++) {
    drawCharBlended(theme, phrase[i], x, y, textColor, alpha);
    x += FONT_W + CHAR_SPACING;
  }
}

void StoryRenderer::drawCharBlended(const IStoryTheme& theme, char c, int x, int y,
                                    uint16_t color, uint8_t alpha) {
  uint8_t index = fontIndex(c);
  const uint8_t* glyph = theme.getPhraseFont()[index];

  for (int col = 0; col < FONT_W; col++) {
    uint8_t bits = pgm_read_byte(&glyph[col]);

    for (int row = 0; row < FONT_H; row++) {
      if (bits & (1 << row)) {
        int px = x + col;
        int py = y + row;

        if (px >= 0 && px < 64 && py >= 0 && py < 64) {
          uint16_t bg = fbGet(px, py);
          uint16_t blended = blend565(bg, color, alpha);
          fbSet(px, py, blended);
        }
      }
    }
  }
}

int StoryRenderer::textWidth(const char* s) const {
  if (!s) return 0;
  return strlen(s) * (FONT_W + CHAR_SPACING) - CHAR_SPACING;
}

void StoryRenderer::drawTime(const Act& activeAct, uint8_t hour, uint8_t minute, uint32_t now,
                             uint16_t color) {
  FB_GFX fb_gfx(framebuffer_);
  fb_gfx.setTextSize(1);
  fb_gfx.setTextColor(color);
  fb_gfx.setCursor(activeAct.getLayout().timeX, activeAct.getLayout().timeY);

  char buf[6];
  bool showColon = (now / 1000) % 2;
  snprintf(buf, sizeof(buf), showColon ? "%02d:%02d" : "%02d %02d", hour, minute);
  fb_gfx.print(buf);
}

}  // namespace story