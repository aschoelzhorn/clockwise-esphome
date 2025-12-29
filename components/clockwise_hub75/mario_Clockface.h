#pragma once

#include "mario_Super_Mario_Bros__24pt7b.h"

#include <Adafruit_GFX.h>
#include "Tile.h"
#include "Locator.h"
#include "Game.h"
#include "Object.h"
#include "ImageUtils.h"
#include "EventBus.h"
// Commons
#include "IClockface.h"
#include "CWDateTime.h"

#include "mario_assets.h"
#include "mario.h"
#include "mario_goomba.h"
#include "mario_block.h"

namespace mario {
class Clockface : public IClockface {
  private:
    Adafruit_GFX* _display;
    CWDateTime* _dateTime;
    void updateTime();
    bool shouldBeNightMode();
    void applyNightMode(bool enable);
    void drawStars();
    void drawStaticObjects();
    // void drawTransparent(int x, int y, const uint16_t* bitmap, int width, int height, uint16_t maskColor);
    unsigned long lastMillis = 0;
    bool _isNightMode = false;

    // Member objects instead of globals
    EventBus* eventBus;
    Tile* ground;
    Object* bush;
    Object* cloud1;
    Object* cloud2;
    Object* hill;
    Object* moon;
    Mario* mario;
    Goomba* goomba;
    Block* hourBlock;
    Block* minuteBlock;

public:
    Clockface(Adafruit_GFX* display);
    ~Clockface();
    void setup(CWDateTime *dateTime);
    void update();
    void externalEvent(int type);

};
}  // namespace mario
