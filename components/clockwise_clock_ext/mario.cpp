#include "mario.h"
#include "esphome/core/log.h"

static const char *TAG = "mario";

Mario::Mario(int x, int y) {
  _x = x;
  _y = y;
}

void Mario::move(Direction dir, int times) {
  
  if (dir == RIGHT) {
    _x += MARIO_PACE;
  } else if (dir == LEFT) {
    _x -= MARIO_PACE;
  }  

}

void Mario::jump(bool shouldHitBlocks) {
  if (_state != JUMPING && (millis() - lastMillis > 500) ) {
    // Serial.println("Jump - Start");

    _lastState = _state;
    _state = JUMPING;
    _shouldHitBlocks = shouldHitBlocks;  // Store whether this jump should hit blocks

    // Clear Mario's current position using his current size
    Locator::getDisplay()->fillRect(_x, _y, _width, _height, SKY_COLOR);
    
    _width = MARIO_JUMP_SIZE[0];
    _height = MARIO_JUMP_SIZE[1];
    _sprite = MARIO_JUMP;

    _direction = UP;

    _lastY = _y;
    _lastX = _x;
  }  
}

void Mario::idle() {
  if (_state != IDLE) {
    // Serial.println("Idle - Start");

    _lastState = _state;
    _state = IDLE;

    Locator::getDisplay()->fillRect(_x, _y, _width, _height, SKY_COLOR);

    _width = MARIO_IDLE_SIZE[0];
    _height = MARIO_IDLE_SIZE[1];
    _sprite = MARIO_IDLE;
  }
}


void Mario::init() {
  Locator::getEventBus()->subscribe(this);
  Locator::getDisplay()->drawRGBBitmap(_x, _y, MARIO_IDLE, MARIO_IDLE_SIZE[0], MARIO_IDLE_SIZE[1]);
}

void Mario::update() {
  

  if (_state == IDLE && _state != _lastState) {
    Locator::getDisplay()->drawRGBBitmap(_x, _y, MARIO_IDLE, MARIO_IDLE_SIZE[0], MARIO_IDLE_SIZE[1]);
  } else if (_state == JUMPING) {
    
    if (millis() - lastMillis >= 100) {  // Slowed down from 50ms to 100ms

      //Serial.println(_y);
      
      Locator::getDisplay()->fillRect(_x, _y, _width, _height, SKY_COLOR);
      
      _y = _y + (MARIO_PACE * (_direction == UP ? -1 : 1));

      Locator::getDisplay()->drawRGBBitmap(_x, _y, _sprite, _width, _height);
      
      // Broadcast different events based on whether this jump should hit blocks
      if (_shouldHitBlocks) {
        Locator::getEventBus()->broadcast(MOVE, this);  // Normal move - blocks will check collision
      } else {
        Locator::getEventBus()->broadcast(MOVE_NO_HIT, this);  // Collision avoidance move - blocks ignore
      }

     
      if (floor(_lastY - _y) >= MARIO_JUMP_HEIGHT) {
        _direction = DOWN;
      // }

      // if (_direction == DOWN) {
        // We should redraw blocks here when Mario is descending, but only on no collision jumps
        ESP_LOGD(TAG, "broadcast(REDRAW_BLOCK), mario going down");
        Locator::getEventBus()->broadcast(REDRAW_BLOCK, this);
      }

      if (_y+_height >= 56) {
        idle();
      }

      lastMillis = millis();
    }

  }
}

void Mario::execute(EventType event, Sprite* caller) {
  if (event == EventType::COLLISION_JUMP) {
    ESP_LOGD(TAG, "Collision jump triggered - no time update!");
    jump(false);  // Collision jump - don't hit blocks
  }
}

const char* Mario::name() {
  return "MARIO";
}