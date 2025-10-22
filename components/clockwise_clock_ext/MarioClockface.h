#pragma once

#include "Super_Mario_Bros__24pt7b.h"

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
#include "goomba.h"
#include "block.h"

class MarioClockface : public IClockface, public EventTask {
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
    MarioClockface(Adafruit_GFX* display);
    ~MarioClockface();
    void setup(CWDateTime *dateTime);
    void update();
    void externalEvent(int type);
    void execute(EventType event, Sprite* caller);
    const char* name();

};
