#include "mario_goomba.h"
#include "esphome/core/log.h"

namespace mario {

static const char *TAG = "goomba";

Goomba::Goomba(int x, int y) {
  _x = x;
  _y = y;
  _width = GOOMBA_SIZE[0];
  _height = GOOMBA_SIZE[1];
  _sprite = GOOMBA;
}

void Goomba::move(Direction dir, int times) {
  // Store old position
  int oldX = _x;
  int oldY = _y;
  
  // Calculate new position
  if (dir == RIGHT) {
    _x += GOOMBA_PACE;
  } else if (dir == LEFT) {
    _x -= GOOMBA_PACE;
  }  
  
  // Only clear and redraw if the old position was visible
  if (oldX >= -8 && oldX < 64) {
    redrawBackground(oldX, oldY, _width, _height);
  }
  
  // Store current position for next clear
  _lastX = _x;
  _lastY = _y;
}

void Goomba::drawTransparent(int x, int y, const uint16_t* bitmap, int width, int height) {
  // Draw bitmap pixel by pixel, skipping mask pixels
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      uint16_t pixel = pgm_read_word(&bitmap[j * width + i]);
      if (pixel != _MASK) {  // Only draw non-transparent pixels
        Locator::getDisplay()->drawPixel(x + i, y + j, pixel);
      }
    }
  }
}

void Goomba::checkMarioCollision() {
  // Mario is at position (23, 40), Goomba is at (_x, 48)
  // Check if Goomba is approaching Mario horizontally
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
    ESP_LOGD(TAG, "Distance to Mario: %d, _movingRight: %s, Approaching: %s, x/y: %d/%d", 
             distance, _movingRight ? "true" : "false", approaching ? "true" : "false", _x, _y);
    
    // Trigger Mario jump through the event bus
    ESP_LOGD(TAG, "Broadcasting COLLISION_JUMP event");
    Locator::getEventBus()->broadcast(COLLISION_JUMP, this);
    _lastJumpTrigger = millis();  // Set cooldown
  }
}

void Goomba::redrawBackground(int x, int y, int width, int height) {
  // Fill with sky color first
  Locator::getDisplay()->fillRect(x, y, width, height, SKY_COLOR);
  
  // Redraw background elements that might be in this area
  
  // Bush is at position (43, 47) with size (21, 9)
  // Check if the cleared area overlaps with the bush
  if (x < 43 + 21 && x + width > 43 && y < 47 + 9 && y + height > 47) {
    // Redraw the part of the bush that was cleared
    Locator::getDisplay()->drawRGBBitmap(43, 47, BUSH, 21, 9);
  }
  
  // Hill is at position (0, 34) with size (20, 22) - might overlap at edges
  if (x < 0 + 20 && x + width > 0 && y < 34 + 22 && y + height > 34) {
    // Redraw hill if it overlaps
    Locator::getDisplay()->drawRGBBitmap(0, 34, HILL, 20, 22);
  }
  
  // Ground tiles (y=56 and below)
  if (y + height > 56) {
    int startX = (x / 8) * 8;
    int endX = ((x + width + 7) / 8) * 8;
    
    for (int gx = startX; gx < endX; gx += 8) {
      if (gx >= 0 && gx < 64) {
        Locator::getDisplay()->drawRGBBitmap(gx, 56, GROUND, 8, 8);
      }
    }
  }
}

void Goomba::walk() {
  if (_state != WALKING) {
    _lastState = _state;
    _state = WALKING;
  }
  
  // Check boundaries - hide when reaching edge instead of turning around
  if ((_movingRight && _x >= 64) || (!_movingRight && _x <= -8)) {
    hide();  // Hide when reaching edge
    return;
  }
  
  // Move the Goomba
  move(_direction, 1);
}

void Goomba::idle() {
  if (_state != IDLE) {
    _lastState = _state;
    _state = IDLE;

    Locator::getDisplay()->fillRect(_x, _y, _width, _height, SKY_COLOR);

    _width = GOOMBA_SIZE[0];
    _height = GOOMBA_SIZE[1];
    _sprite = GOOMBA;
  }
}

void Goomba::hide() {
  if (_state != HIDDEN) {
    _lastState = _state;
    _state = HIDDEN;
    
    // Clear Goomba from screen
    redrawBackground(_x, _y, _width, _height);
    
    // Schedule next run (20-60 seconds from now)
    _nextRunTime = millis() + random(20000, 60000);
    ESP_LOGD(TAG, "Goomba hidden, next run in %lu ms", _nextRunTime - millis());
  }
}

void Goomba::startRandomRun() {
  // Randomly choose direction
  _movingRight = random(0, 2) == 0;  // 50% chance
  
  if (_movingRight) {
    _x = -8;  // Start from left
    _direction = RIGHT;
  } else {
    _x = 64;  // Start from right
    _direction = LEFT;
  }
  
  _y = 48;  // Ground level
  _lastX = _x;
  _lastY = _y;
  
  _state = WALKING;
  _lastState = HIDDEN;
  
  ESP_LOGD(TAG, "Goomba starting run from %s", _movingRight ? "left" : "right");
}

bool Goomba::isNearTimeChange() {
  // Check if we're within 10 seconds before or 5 seconds after a minute change
  // This avoids interfering with Mario's time jump
  if (_dateTime == nullptr) return false;

  int seconds = _dateTime->getSecond();
  
  // Avoid 50-59 seconds (before minute change) and 0-5 seconds (after minute change)
  return (seconds >= 50 || seconds <= 5);
}


void Goomba::init(CWDateTime* dateTime) {
  Locator::getEventBus()->subscribe(this);
  _lastX = _x;
  _lastY = _y;
  _dateTime = dateTime;
  // Don't draw initially if starting off-screen
  if (_x > -8 && _x < 64) {
    drawTransparent(_x, _y, GOOMBA, GOOMBA_SIZE[0], GOOMBA_SIZE[1]);
  }
}

void Goomba::update() {
  // Check if it's time to start a new run when hidden
  if (_state == HIDDEN) {
    if (millis() >= _nextRunTime && !isNearTimeChange()) {
      startRandomRun();
    }
    return;  // Nothing to update while hidden
  }
  
  // Movement timing - update every 200ms for smooth movement
  if (millis() - _lastMillis >= 200) {
    
    if (_state == WALKING) {
      walk();
      // Check for Mario collision and trigger jump
      checkMarioCollision();
      
      // Only draw the Goomba if it's visible (partially or fully on screen)
      if (_x > -8 && _x < 64) {
        drawTransparent(_x, _y, GOOMBA, GOOMBA_SIZE[0], GOOMBA_SIZE[1]);
      }
    } else if (_state == IDLE && _state != _lastState) {
      if (_x > -8 && _x < 64) {
        drawTransparent(_x, _y, GOOMBA, GOOMBA_SIZE[0], GOOMBA_SIZE[1]);
      }
    }
    
    _lastMillis = millis();
  }
}

void Goomba::execute(EventType event, Sprite* caller) {

}

const char* Goomba::name() {
  return "GOOMBA ";
}

}  // namespace mario