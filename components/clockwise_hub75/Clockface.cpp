#include "Clockface.h"


unsigned long lastMillis = 0;
unsigned long lastMillisTime = 0;
unsigned long lastMillisSec = 0;


Pacman *pacman;

Clockface::Clockface(Adafruit_GFX* display) {
  _display = display;
  Locator::provide(display);
}

void Clockface::setup(CWDateTime *dateTime) {
  this->_dateTime = dateTime;
  Locator::getDisplay()->setFont(&hourFont);
  randomSeed(dateTime->getMilliseconds() + millis());
  drawMap();
  updateClock();
}

void Clockface::update()
{

  // Seconds blink  
  if ((millis() - lastMillisSec) >= 1000) {
    
    if (show_seconds) {
      Locator::getDisplay()->fillRect(31, 24, 2, 2, 0xFE40);
      Locator::getDisplay()->fillRect(31, 29, 2, 2, 0xFE40);
    } else  {
      Locator::getDisplay()->fillRect(31, 24, 2, 2, 0);
      Locator::getDisplay()->fillRect(31, 29, 2, 2, 0);
    }

    show_seconds = !show_seconds;
    lastMillisSec = millis();
  }

  // Clock
  if (millis() - lastMillisTime >= 60000) {

    updateClock();
    
    lastMillisTime = millis();
  }


  // Pacman
  if (millis() - lastMillis >= 75) {
    
    bool fullBlock = // X axis
                     ((pacman->_direction == Direction::LEFT || pacman->_direction == Direction::RIGHT) && (pacman->getX()-2) % 5 == 0) ||
                     // Y axis
                     ((pacman->_direction == Direction::UP || pacman->_direction == Direction::DOWN) && (pacman->getY()-2) % 5 == 0);

    
    if (fullBlock) {
      
      MapBlock nextBlk = nextBlock();

      //change block to empty where pacman passes
      _MAP[(pacman->getY()-2)/5][(pacman->getX()-2)/5] = MapBlock::EMPTY;

      directionDecision(nextBlk, (pacman->_direction == Direction::LEFT || pacman->_direction == Direction::RIGHT));

      if (nextBlk == MapBlock::SUPER_FOOD) {
        pacman->setState(Pacman::State::INVENCIBLE);
      }

      if (countBlocks(MapBlock::FOOD) == 0) {
        resetMap();
      }
    }


    pacman->update();

    lastMillis = millis();
  }

  
  
}


const char* Clockface::weekDayName(int weekday) {
  strncpy(weekDayTemp, _weekDayWords + (weekday*4), 4);
  return weekDayTemp;
}

const char* Clockface::monthName(int month) {
  strncpy(monthTemp, _monthWords + ((month-1)*4), 4);
  return monthTemp;
}



void Clockface::updateClock() {

    Locator::getDisplay()->fillRect(14, 19, 36, 26, 0x0000);

    Locator::getDisplay()->setFont(&Picopixel);
    Locator::getDisplay()->setTextColor(0xAD55);
    Locator::getDisplay()->setCursor(15, 41);
    Locator::getDisplay()->print(monthName(this->_dateTime->getMonth()));
    Locator::getDisplay()->print(" ");
    Locator::getDisplay()->print(this->_dateTime->getDay());
    Locator::getDisplay()->print(" ");
    Locator::getDisplay()->print(weekDayName(this->_dateTime->getWeekday()));
    
    Locator::getDisplay()->setFont(&hourFont);
    
    Locator::getDisplay()->setTextColor(0xFE40);
    Locator::getDisplay()->setCursor(15, 28);
    
    Locator::getDisplay()->print(this->_dateTime->getHour("00"));
    Locator::getDisplay()->print(" ");
    Locator::getDisplay()->print(this->_dateTime->getMinute("00"));
}

void Clockface::directionDecision(MapBlock nextBlk, bool moving_axis_x) {

  if (contains(nextBlk, PACMAN_BLOCKING_BLOCKS)) {
    turnRandom();
  } else if (moving_axis_x && contains(nextBlock(Direction::DOWN), PACMAN_MOVING_BLOCKS) && random(100) % 2 == 0) {
    pacman->turn(Direction::DOWN);
  } else if (moving_axis_x && contains(nextBlock(Direction::UP), PACMAN_MOVING_BLOCKS) && random(100) % 2 == 0) {
    pacman->turn(Direction::UP);
  } else if (!moving_axis_x && contains(nextBlock(Direction::LEFT), PACMAN_MOVING_BLOCKS) && random(100) % 2 == 0) {
    pacman->turn(Direction::LEFT);
  } else if (!moving_axis_x && contains(nextBlock(Direction::RIGHT), PACMAN_MOVING_BLOCKS) && random(100) % 2 == 0) {
    pacman->turn(Direction::RIGHT);
  }

}


void Clockface::resetMap() {

  memcpy( _MAP, _MAP_CONST, sizeof(_MAP_CONST) );
  drawMap();
  updateClock();
}


int Clockface::countBlocks(Clockface::MapBlock elem) {
  int count = 0;
  for (int i = 0; i<MAP_SIZE; i++) {
    for (int j = 0; j<MAP_SIZE; j++) {
      if (_MAP[i][j] == elem)
        count++;
    }
  }

  return count;
}


void Clockface::turnRandom() {
  int dir = random(4);
  //int dir = 3;
  //pacman->_state = Pacman::State::TURNING;

  do {
    pacman->turn(static_cast<Direction>(dir));
    dir = random(4);
    //dir++;

    
  } while (!contains(nextBlock(), PACMAN_MOVING_BLOCKS));

  // Serial.print("New direction: ");
  // Serial.println(pacman->_direction);
}


Clockface::MapBlock Clockface::nextBlock() {
  return nextBlock(pacman->_direction);
}

Clockface::MapBlock Clockface::nextBlock(Direction dir) {

  Clockface::MapBlock map_block = Clockface::MapBlock::OUT_OF_MAP;

  if (dir == Direction::RIGHT) {
    if (pacman->getX()+pacman->SPRITE_SIZE < MAP_MAX_POS) {
      map_block = static_cast<MapBlock>(_MAP[(pacman->getY()-2)/5][((pacman->getX()-2)/5)+1]);
    }
    
  } else if (dir == Direction::DOWN) {
    if (pacman->getY()+pacman->SPRITE_SIZE < MAP_MAX_POS) {
      map_block = static_cast<MapBlock>(_MAP[((pacman->getY()-2)/5)+1][(pacman->getX()-2)/5]);
    }
  } else if (dir == Direction::LEFT) {

    if ((pacman->getX()-2) > 0) {
      map_block = static_cast<MapBlock>(_MAP[(pacman->getY()-2)/5][((pacman->getX()-2)/5)-1]);
    }

  } else if (dir == Direction::UP) {
    if ((pacman->getY()-2) > 0) {
      map_block = static_cast<MapBlock>(_MAP[((pacman->getY()-2)/5)-1][((pacman->getX()-2)/5)]);
    }
  }

  return map_block;

}

bool Clockface::contains(int v, const int* values) {
  
  for (int i = 1; i<values[0]+1; i++) {
    if (v == values[i])
      return true;
  }

  return false;
}

void Clockface::drawMap() 
{
  //Locator::getDisplay()->drawRGBBitmap(0, 0, _PACMAN_MAP, 64, 64);
  //Locator::getDisplay()->getPixel(0, 0);

  Locator::getDisplay()->fillRect(0, 0, 64, 64, 0x0000);



  uint16_t food_color = 0xB58C;
  uint16_t wall_color = 0x0016;
  uint16_t spcfood_color = 0xFBE0;



  Locator::getDisplay()->drawRect(0,0,64,64,wall_color);
  Locator::getDisplay()->drawRect(1,1,62,62,wall_color);


  for (int i=0; i<MAP_SIZE; i++) {
    for (int j=0; j<MAP_SIZE; j++) {
      if (_MAP[j][i] == MapBlock::FOOD) {
        Locator::getDisplay()->fillRect((i*5)+3,(j*5)+4,3,1,food_color);
      } else if (_MAP[j][i] == MapBlock::WALL) {
        Locator::getDisplay()->fillRect((i*5)+2,(j*5)+2,5,5,wall_color);
      } else if (_MAP[j][i] == MapBlock::CLOCK) {
        Locator::getDisplay()->fillRect((i*5)+2,(j*5)+2,5,5,wall_color);
      } else if (_MAP[j][i] == MapBlock::GATE) {
        //Locator::getDisplay()->fillRect((i*5)+((bool)i*2),(j*5)+2,7,5,0x0000);
        Locator::getDisplay()->fillRect((i*5)+3,(j*5)+4,3,1,food_color);
      } else if (_MAP[j][i] == MapBlock::SUPER_FOOD) {
        Locator::getDisplay()->fillRect((i*5)+3,(j*5)+3,3,3,spcfood_color);
      } else if (_MAP[j][i] == MapBlock::PACMAN) {
        pacman = new Pacman((i*5)+2,(j*5)+2);

        // Locator::getDisplay()->drawRGBBitmap((i*5)+2,(j*5)+2, _PACMAN_2, 5, 5);
        // pacmanState = !pacmanState;
      }
    }
  }

  
}
