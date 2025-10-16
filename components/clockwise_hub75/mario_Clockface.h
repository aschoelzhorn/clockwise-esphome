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
    unsigned long lastMillis = 0;

    // Member objects instead of globals
    EventBus* eventBus;
    Tile* ground;
    Object* bush;
    Object* cloud1;
    Object* cloud2;
    Object* hill;
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
