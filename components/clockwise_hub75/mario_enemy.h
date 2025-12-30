#pragma once

#include <Arduino.h>
#include "Game.h"
#include "Locator.h"
#include "EventBus.h"
#include "ImageUtils.h"
#include "mario_assets.h"
#include "CWDateTime.h"

namespace mario {
enum class EnemyType {
  GOOMBA,
  KOOPA
};

const uint8_t ENEMY_PACE = 6;

class Enemy: public Sprite, public EventTask {
  private:

    enum State {
      HIDDEN,   // Not visible, waiting for next run
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
    State _state = HIDDEN;              // Start hidden
    State _lastState = HIDDEN; 

    const char* _name;
    
    void walk();
    void hide();

  public:
    Enemy(int x, int y, EnemyType type);
    void init();
    void move(Direction dir);
    void update();
    void redrawBackground(int x, int y, int width, int height); // Method to redraw background
    void checkMarioCollision(); // Check if close to Mario and make him jump
    void startRandomRun();
    const char* name() override;
    void execute(EventType event, Sprite* caller);
    bool isHidden() const { return _state == HIDDEN; }
};

}  // namespace mario
