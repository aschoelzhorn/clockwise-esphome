#include "mario_enemy.h"
#include "esphome/core/log.h"

namespace mario {

Enemy::Enemy(int x, int y, EnemyType type) {
  _x = x;
  _y = y;
  _width = ENEMY_SIZE[0];
  _height = ENEMY_SIZE[1];
  _state = HIDDEN;
  _lastState = HIDDEN;
  switch (type) {
    case EnemyType::GOOMBA:
      _sprite = GOOMBA;
      _name = "GOOMBA";
      break;
    case EnemyType::KOOPA:
      _sprite = KOOPA;
      _name = "KOOPA";
      break;
  }
}

void Enemy::move(Direction dir) {
  // Store old position
  int oldX = _x;
  int oldY = _y;
  
  // Calculate new position
  if (dir == RIGHT) {
    _x += ENEMY_PACE;
  } else if (dir == LEFT) {
    _x -= ENEMY_PACE;
  }  
  
  // Only clear and redraw if the old position was visible
  if (oldX >= -_width && oldX < DISPLAY_WIDTH) {
    redrawBackground(oldX, oldY, _width, _height);
  }
  
  // Store current position for next clear
  _lastX = _x;
  _lastY = _y;
}

void Enemy::checkMarioCollision() {
  // Mario is at position (23, 40), Enemy is at (_x, 48)
  // Check if enemy is approaching Mario horizontally
  const int MARIO_X = 23;
  const int TRIGGER_DISTANCE_LEFT = 7;   // Coming from left - works at distance 7
  const int TRIGGER_DISTANCE_RIGHT = 17; // Coming from right - needs to trigger 6 pixels earlier (11+6)
  const int JUMP_COOLDOWN = 2000;   // 2 second cooldown between jumps
  
  // Only trigger if enough time has passed since last jump
  if (millis() - _lastJumpTrigger < JUMP_COOLDOWN) {
    return;
  }
  
  // Calculate distance and determine which trigger distance to use
  int distance = abs(_x - MARIO_X);
  bool approaching = false;
  int triggerDistance = TRIGGER_DISTANCE_LEFT;
  
  if (_movingRight && _x < MARIO_X) {
    approaching = true;  // Moving right towards Mario
    triggerDistance = TRIGGER_DISTANCE_LEFT;
  } else if (!_movingRight && _x > MARIO_X) {
    approaching = true;  // Moving left towards Mario
    triggerDistance = TRIGGER_DISTANCE_RIGHT;  // Larger distance to trigger earlier
  }
  
  if (approaching && distance <= triggerDistance) {
    ESP_LOGD(name(), "Distance to Mario: %d, _movingRight: %s, Approaching: %s, x/y: %d/%d", 
             distance, _movingRight ? "true" : "false", approaching ? "true" : "false", _x, _y);
    
    // Trigger Mario jump through the event bus
    ESP_LOGD(name(), "Broadcasting COLLISION_JUMP event");
    Locator::getEventBus()->broadcast(COLLISION_JUMP, this);
    _lastJumpTrigger = millis();  // Set cooldown
  }
}

void Enemy::redrawBackground(int x, int y, int width, int height) {
  // Fill with sky color first
  Locator::getDisplay()->fillRect(x, y, width, height, SKY_COLOR_NIGHT);
  
  // Redraw background elements that might be in this area
  
  // Bush is at position (43, 47) with size (21, 9)
  // Check if the cleared area overlaps with the bush
  if (x < 43 + 21 && x + width > 43 && y < 47 + 9 && y + height > 47) {
    // Redraw the part of the bush that was cleared
    ImageUtils::drawTransparent(43, 47, BUSH, 21, 9, SKY_COLOR_NIGHT); // Use SKY_COLOR or SKY_COLOR_NIGHT
  }
  
  // Hill is at position (0, 34) with size (20, 22) - might overlap at edges
  if (x < 0 + 20 && x + width > 0 && y < 34 + 22 && y + height > 34) {
    // Redraw hill if it overlaps
    ImageUtils::drawTransparent(0, 34, HILL, 20, 22, SKY_COLOR_NIGHT); // Use SKY_COLOR or SKY_COLOR_NIGHT
  }
}

void Enemy::walk() {
  if (_state != WALKING) {
    _lastState = _state;
    _state = WALKING;
  }
  
  // Check boundaries - hide when reaching edge instead of turning around
  if ((_movingRight && _x >= DISPLAY_WIDTH) || (!_movingRight && _x <= -_width)) {
    hide();  // Hide when reaching edge
    return;
  }
  
  // Move the Enemy
  move(_direction);
}

void Enemy::hide() {
  if (_state != HIDDEN) {
    _lastState = _state;
    _state = HIDDEN;
    
    // Clear Enemy from screen
    redrawBackground(_x, _y, _width, _height);
  }
}

void Enemy::startRandomRun() {
  // Randomly choose direction
  _movingRight = random(0, 2) == 0;  // 50% chance
  
  if (_movingRight) {
    _x = -_width;  // Start from left
    _direction = RIGHT;
  } else {
    _x = DISPLAY_WIDTH;  // Start from right
    _direction = LEFT;
  }
  
  _y = 48;  // Ground level
  _lastX = _x;
  _lastY = _y;
  
  _state = WALKING;
  _lastState = HIDDEN;
  
  ESP_LOGD(name(), "Enemy starting run from %s", _movingRight ? "left" : "right");
}

void Enemy::init() {
  Locator::getEventBus()->subscribe(this);
  _lastX = _x;
  _lastY = _y;
  // Don't draw initially if starting off-screen
  if (_x > -_width && _x < DISPLAY_WIDTH) {
    ImageUtils::drawTransparent(_x, _y, _sprite, _width, _height, SKY_COLOR_NIGHT);
  }
}

void Enemy::update() {

  if (_state == HIDDEN) {
    return;  // Nothing to update while hidden
  }
  
  // Movement timing - update every 200ms for smooth movement
  if (millis() - _lastMillis >= 200) {
    
    if (_state == WALKING) {
      walk();
      // Check for Mario collision and trigger jump
      checkMarioCollision();
      
      // Only draw the Enemy if it's visible (partially or fully on screen)
      if (_x > -_width && _x < DISPLAY_WIDTH) {
        ImageUtils::drawTransparent(_x, _y, _sprite, _width, _height, SKY_COLOR_NIGHT);
      }
    } 
    /*else if (_state == IDLE && _state != _lastState) {
      if (_x > -_width && _x < DISPLAY_WIDTH) {
        ImageUtils::drawTransparent(_x, _y, _sprite, _width, _height, SKY_COLOR_NIGHT);
      }
    }*/
    
    _lastMillis = millis();
  }
}

void Enemy::execute(EventType event, Sprite* caller) {
  // Currently no event handling needed for Enemy
 }  

const char* Enemy::name() {
  return _name;
}

}  // namespace mario