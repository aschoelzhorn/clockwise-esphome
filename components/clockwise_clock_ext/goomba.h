#pragma once

#include <Arduino.h>
#include "Game.h"
#include "Locator.h"
#include "EventBus.h"
#include "ImageUtils.h"
#include "mario_assets.h"

const uint8_t GOOMBA_PACE = 1;

class Goomba: public Sprite, public EventTask {
  private:

    enum State {
      IDLE,
      WALKING
    };

    Direction direction;

    int _lastX;
    int _lastY;

    const unsigned short* _sprite;
    unsigned long lastMillis = 0;
    State _state = IDLE; 
    State _lastState = IDLE; 
    
    void idle();

  public:
    Goomba(int x, int y);
    void init();
    void move(Direction dir, int times);
    void update();
    const char* name();
    void execute(EventType event, Sprite* caller);
    
};
