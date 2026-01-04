#include "pacman.h"

Pacman::Pacman(int x, int y) {
  _x = x;
  _y = y;
}

void Pacman::turn(Direction dir) {

  // set to right  
  memcpy( _PACMAN, _PACMAN_CONST, sizeof(_PACMAN) );
  
  if (dir == Direction::LEFT) {
    flip();
  } else if (dir == Direction::DOWN) {
    flip();
    rotate();    
    flip();
  } else if (dir == Direction::UP) {
    rotate();
  }

  changePacmanColor(current_color);

  _direction = dir;

}

void Pacman::move(Direction dir) {
  
  if (dir == Direction::RIGHT) {
    _x += 1;
  } else if (dir == Direction::LEFT) {
    _x -= 1;
  } else if (dir == Direction::DOWN) {
    _y += 1;
  } else if (dir == Direction::UP) {
    _y -= 1;
  }

}


void Pacman::init() {
  //Locator::getEventBus()->subscribe(this);
  Locator::getDisplay()->drawRGBBitmap(_x, _y, _PACMAN[int(_pacman_anim)], SPRITE_SIZE, SPRITE_SIZE);
}

void Pacman::update() { 
  
  if (_state == MOVING || _state == INVENCIBLE) {
    Locator::getDisplay()->fillRect(_x,_y, SPRITE_SIZE, SPRITE_SIZE, 0);
    this->move(_direction);
  }

  if (_iteration % 3 == 0)
    _pacman_anim = !_pacman_anim; 


  if (_state == INVENCIBLE) {
    
    if (_iteration % 2 == 0) {
      current_color = random(LONG_MAX);
    } else {
      current_color = 0xFE40;
    }
    
    if ((millis() - invencibleTimeout) >= 7000) {
      _state = MOVING;
      current_color = 0xFE40;
    }

    changePacmanColor(current_color);
  }
  
  Locator::getDisplay()->drawRGBBitmap(_x, _y, _PACMAN[int(_pacman_anim)], SPRITE_SIZE, SPRITE_SIZE);

  _iteration++;
}

void Pacman::setState(State state) {

  if (state == INVENCIBLE) {
    invencibleTimeout = millis();
    randomSeed(millis());
  }

  _state = state;
}


//TODO move to the gfx-engine lib
void Pacman::rotate() {
  int n = SPRITE_SIZE;

  uint16_t temp0[2][25] = {};  
  memcpy( temp0, _PACMAN, sizeof(temp0) );
 
  for(int i = 0; i < n; i++) {
    for(int j = 0; j < n; j++) {
      _PACMAN[0][(i*n)+j] = temp0[0][(n-1-i)+(j*n)];
      _PACMAN[1][(i*n)+j] = temp0[1][(n-1-i)+(j*n)];
    }
  }
}

//TODO move to the gfx-engine lib
void Pacman::flip() {
  int n = SPRITE_SIZE;

  for(int i = 0; i < n; i++) {
    for(int j = 0; j < floor(n/2)+1; j++) {
      std::swap(_PACMAN[0][j + (i*n)], _PACMAN[0][(i*n)+n-j-1]);
      std::swap(_PACMAN[1][j + (i*n)], _PACMAN[1][(i*n)+n-j-1]);
    }
  }

}

void Pacman::execute(EventType event, Sprite* caller) {
  if (event == EventType::COLLISION) {
    _direction = DOWN;
  }
}

void Pacman::changePacmanColor(uint16_t newcolor) {
  for (int i=0; i < SPRITE_SIZE*SPRITE_SIZE; i++) {
    if (_PACMAN[0][i] != 0x0000) 
      _PACMAN[0][i] = newcolor;
    if (_PACMAN[1][i] != 0x0000)
      _PACMAN[1][i] = newcolor;
  }

}

int Pacman::getX() {
  return this->_x;
}
int Pacman::getY() {
  return this->_y;
}

const char* Pacman::name() {
  return "PACMAN";
}