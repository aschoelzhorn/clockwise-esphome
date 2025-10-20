#include "goomba.h"

Goomba::Goomba(int x, int y) {
  _x = x;
  _y = y;
}

void Goomba::move(Direction dir, int times) {
  
  if (dir == RIGHT) {
    _x += GOOMBA_PACE;
  } else if (dir == LEFT) {
    _x -= GOOMBA_PACE;
  }  

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
  Locator::getDisplay()->drawRGBBitmap(_x, _y, GOOMBA, GOOMBA_SIZE[0], GOOMBA_SIZE[1]);
}

void Goomba::update() {

  if (_state == IDLE && _state != _lastState) {
    Locator::getDisplay()->drawRGBBitmap(_x, _y, GOOMBA, GOOMBA_SIZE[0], GOOMBA_SIZE[1]);
  } //else if (_state == JUMPING) {
    
    // if (millis() - lastMillis >= 50) {

    //   //Serial.println(_y);
      
    //   Locator::getDisplay()->fillRect(_x, _y, _width, _height, SKY_COLOR);
      
    //   _y = _y + (GOOMBA_PACE * (direction == UP ? -1 : 1));

    //   Locator::getDisplay()->drawRGBBitmap(_x, _y, _sprite, _width, _height);
      
    //   Locator::getEventBus()->broadcast(MOVE, this);

    //   // if (_y+_height >= 56) {
    //   //   idle();
    //   // }

    //   lastMillis = millis();
    //}

  //}
}

void Goomba::execute(EventType event, Sprite* caller) {
  //if (event == EventType::COLLISION) {
    //Serial.println("GOOMBA - Collision detected");
    //direction = DOWN;
  //}
}

const char* Goomba::name() {
  return "GOOMBA ";
}