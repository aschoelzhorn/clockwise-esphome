#pragma once

#include <Arduino.h>
#include "Game.h"
#include "Locator.h"
#include "EventBus.h"
#include "ImageUtils.h"
#include "mario_assets.h"

const uint8_t GOOMBA_PACE = 3;

class Goomba: public Sprite, public EventTask {
  private:

    enum State {
      IDLE,
      WALKING
    };

    Direction direction = RIGHT;  // Start moving right
    bool movingRight = true;      // Track movement direction

    int _lastX;
    int _lastY;

    const unsigned short* _sprite;
    unsigned long lastMillis = 0;
    State _state = WALKING;       // Start in walking state
    State _lastState = IDLE; 
    
    void idle();
    void walk();

  public:
    Goomba(int x, int y);
    void init();
    void move(Direction dir, int times);
    void update();
    void redrawBackground(int x, int y, int width, int height); // Method to redraw background
    void drawTransparent(int x, int y, const uint16_t* bitmap, int width, int height); // Draw with transparency
    const char* name();
    void execute(EventType event, Sprite* caller);
    
};
