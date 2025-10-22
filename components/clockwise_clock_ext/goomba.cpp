#include "goomba.h"
#include "esphome/core/log.h"

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
  const int MARIO_WIDTH = 16;  // Approximate Mario width
  const int TRIGGER_DISTANCE = 12;  // Distance to trigger jump
  const int JUMP_COOLDOWN = 2000;   // 2 second cooldown between jumps
  
  // Only trigger if enough time has passed since last jump
  if (millis() - lastJumpTrigger < JUMP_COOLDOWN) {
    return;
  }
  
  // Calculate distance between Goomba and Mario
  int distance = abs(_x - MARIO_X);
  
  // Check if Goomba is close to Mario and approaching
  if (distance <= TRIGGER_DISTANCE) {
    // Check if Goomba is actually moving towards Mario
    bool approaching = false;
    if (movingRight && _x < MARIO_X) {
      approaching = true;  // Moving right towards Mario
    } else if (!movingRight && _x > MARIO_X) {
      approaching = true;  // Moving left towards Mario  
    }
    
    ESP_LOGD(TAG, "Distance to Mario: %d, MovingRight: %s, Approaching: %s", 
             distance, movingRight ? "true" : "false", approaching ? "true" : "false");
    
    if (approaching) {
      // Trigger Mario jump through the event bus
      ESP_LOGD(TAG, "Broadcasting COLLISION_JUMP event");
      Locator::getEventBus()->broadcast(COLLISION_JUMP, this);
      lastJumpTrigger = millis();  // Set cooldown
    }
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
  
  // Check boundaries and change direction if needed
  if (movingRight && _x >= 64) {  // Completely off right edge
    movingRight = false;
    _direction = LEFT;
  } else if (!movingRight && _x <= -8) {    // Completely off left edge
    movingRight = true;
    _direction = RIGHT;
  }
  
  // Move the Goomba
  move(_direction, 1);
}

void Goomba::idle() {
  if (_state != IDLE) {
    // Serial.println("Idle - Start");

    _lastState = _state;
    _state = IDLE;

    Locator::getDisplay()->fillRect(_x, _y, _width, _height, SKY_COLOR);

    _width = GOOMBA_SIZE[0];
    _height = GOOMBA_SIZE[1];
    _sprite = GOOMBA;
  }
}


void Goomba::init() {
  Locator::getEventBus()->subscribe(this);
  _lastX = _x;
  _lastY = _y;
  // Don't draw initially if starting off-screen
  if (_x > -8 && _x < 64) {
    drawTransparent(_x, _y, GOOMBA, GOOMBA_SIZE[0], GOOMBA_SIZE[1]);
  }
}

void Goomba::update() {
  // Movement timing - update every 200ms for smooth movement
  if (millis() - lastMillis >= 200) {
    
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
    
    lastMillis = millis();
  }
}

void Goomba::execute(EventType event, Sprite* caller) {

}

const char* Goomba::name() {
  return "GOOMBA ";
}