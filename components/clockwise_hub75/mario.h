#pragma once

#include <Arduino.h>
#include "Game.h"
#include "Locator.h"
#include "EventBus.h"
#include "ImageUtils.h"
#include "mario_assets.h"


const uint8_t MARIO_PACE = 3;
const uint8_t MARIO_JUMP_HEIGHT = 14;
const uint8_t MARIO_COLLISION_JUMP_HEIGHT = 12;  // 2 pixels shorter to avoid block


class Mario: public Sprite, public EventTask {
  private:

    enum State {
      IDLE,
      WALKING,
      JUMPING
    };

    Direction _direction;

    int _lastX;
    int _lastY;

    const unsigned short* _sprite;
    unsigned long lastMillis = 0;
    State _state = IDLE; 
    State _lastState = IDLE;
    bool _shouldHitBlocks = true;  // Track if this jump should hit blocks
    uint8_t _targetJumpHeight = MARIO_JUMP_HEIGHT;  // Track target height for this jump
    
    void idle();

  public:
    Mario(int x, int y);
    void init();
    void move(Direction dir, int times);
    void jump(bool shouldHitBlocks = true);  // Parameter to control block hitting
    void update();
    const char* name();
    void execute(EventType event, Sprite* caller);
    
};
