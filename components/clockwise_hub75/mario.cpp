#include "mario.h"
#include "esphome/core/log.h"

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
    _targetJumpHeight = _shouldHitBlocks ? MARIO_JUMP_HEIGHT : MARIO_COLLISION_JUMP_HEIGHT;

    // Clear Mario's current position using his current size
    Locator::getDisplay()->fillRect(_x, _y, _width, _height, _skyColor);
    
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

    Locator::getDisplay()->fillRect(_x, _y, _width, _height, _skyColor);

    _width = MARIO_IDLE_SIZE[0];
    _height = MARIO_IDLE_SIZE[1];
    _sprite = MARIO_IDLE;
  }
}


void Mario::init() {
  Locator::getEventBus()->subscribe(this);
  idle();  // Ensure Mario starts in IDLE state and is drawn  
}

void Mario::draw() {
  ImageUtils::drawTransparent(_x, _y, _sprite, _width, _height, _skyColor);
}

void Mario::update() {

  if (_state == IDLE && _state != _lastState) {
    draw();
  } else if (_state == JUMPING) {
    
    // Calculate how high we've jumped (distance from start)
    int jumpProgress = abs(_lastY - _y);
    bool inUpperHalf = jumpProgress >= (_targetJumpHeight / 2);
    
    // Slow down the upper half of the jump (150ms vs 100ms) only for non block-hitting jumps
    unsigned long jumpDelay = (!_shouldHitBlocks && inUpperHalf) ? 150 : 100;
    
    if (millis() - lastMillis >= jumpDelay) {
      Locator::getDisplay()->fillRect(_x, _y, _width, _height, _skyColor);
      
      _y = _y + (MARIO_PACE * (_direction == UP ? -1 : 1));

      draw();
      
      // Broadcast different events based on whether this jump should hit blocks
      if (_shouldHitBlocks) {
        Locator::getEventBus()->broadcast(MOVE, this);  // Normal move - blocks will check collision
      } 

      if (floor(_lastY - _y) >= _targetJumpHeight) {
        _direction = DOWN;
      }

      if (_y + _height >= 56) {
        idle();
      }

      lastMillis = millis();
    }

  }
}

void Mario::execute(EventType event, Sprite* caller, uint16_t value) {
  if (event == SKY_COLOR_CHANGED) {
    ESP_LOGD(name(), "_skyColor changed to: %u (old color was: %u)", value, _skyColor);
    _skyColor = value;
  } else if (event == EventType::COLLISION_JUMP) {
    ESP_LOGD(name(), "Collision jump triggered - no time update!");
    jump(false);  // Collision jump - don't hit blocks
  }
}

const char* Mario::name() {
  return "MARIO";
}