#pragma once

#include <Arduino.h>
#include "Game.h"
#include "Locator.h"
#include "EventBus.h"
#include "ImageUtils.h"
#include "mario_assets.h"
#include "CWDateTime.h"

namespace mario {

const uint8_t GOOMBA_PACE = 6;

class Goomba: public Sprite, public EventTask {
  private:

    enum State {
      HIDDEN,   // Not visible, waiting for next run
      IDLE,
      WALKING
    };

    Direction _direction = RIGHT;  // Start moving right
    bool _movingRight = true;      // Track movement direction

    int _lastX;
    int _lastY;

    const unsigned short* _sprite;
    CWDateTime* _dateTime;  // Reference to datetime for time checking
    unsigned long _lastMillis = 0;
    unsigned long _lastJumpTrigger = 0;  // Cooldown for Mario jump
    unsigned long _nextRunTime = 0;      // When to start next run
    State _state = HIDDEN;              // Start hidden
    State _lastState = HIDDEN; 
    
    void idle();
    void walk();
    void hide();
    void startRandomRun();
    bool isNearTimeChange();  // Check if current time is close to minute change

  public:
    Goomba(int x, int y);
    void init(CWDateTime* dateTime);
    void move(Direction dir, int times);
    void update();
    void redrawBackground(int x, int y, int width, int height); // Method to redraw background
    void drawTransparent(int x, int y, const uint16_t* bitmap, int width, int height); // Draw with transparency
    void checkMarioCollision(); // Check if close to Mario and make him jump
    const char* name();
    void execute(EventType event, Sprite* caller);
    
};

}  // namespace mario
