#pragma once

#include <Arduino.h>
#include "Game.h"
#include "Locator.h"
#include "EventTask.h"
#include "mario_assets.h"
#include "ImageUtils.h"

const uint8_t MOVE_PACE = 2;
const uint8_t MAX_MOVE_HEIGHT = 4;

class Block: public Sprite, public EventTask {
  private:
    enum State {
      UNKNOWN,
      IDLE,
      HIT
    };   

    Direction _direction; 

    String _text;

    unsigned long lastMillis = 0;
    State _state = UNKNOWN;
    State _lastState = UNKNOWN;
    uint8_t _lastY;
    uint8_t _firstY;
    unsigned short _skyColor = SKY_COLOR;
    
    void idle();
    void hit();
    void setTextBlock();
    void draw();

  public:
    Block(int x, int y);
    void setText(String text);
    void init();
    void update();    
    const char* name();
    void execute(EventType event, Sprite* caller, uint16_t value = 0) override;

};
