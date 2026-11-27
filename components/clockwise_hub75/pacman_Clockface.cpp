#include "pacman_Clockface.h"
#include "SpecialDates.h"
#include "esphome/core/log.h"

namespace pacman {
Clockface::Clockface(Adafruit_GFX* display) {
  _display = display;
  Locator::provide(display);
}

Clockface::~Clockface() {
  if (pacman != nullptr) {
    delete pacman;
    pacman = nullptr;
  }
}

void Clockface::setup(CWDateTime *dateTime) {
  this->_dateTime = dateTime;
  if (dateCanvas.getBuffer() == nullptr) {
    ESP_LOGE("pacman_Clockface", "Unable to allocate special-day text canvas");
  }
  dateCanvas.setFont(&Picopixel);
  dateCanvas.setTextWrap(false);
  dateCanvas.setTextColor(1);
  specialName = special_date_today_name();
  lastMillisScroll = millis();
  Locator::getDisplay()->setFont(&hourFont);
  randomSeed(dateTime->getMilliseconds() + millis());
  drawMap();
  updateClock();
}

void Clockface::update()
{
  const char* name = special_date_today_name();
  if (specialName != name) {
    if (!specialName.empty() && name[0] == '\0') {
      restoreBalloon();
      pacman->init();
    } else if (specialName.empty()) {
      balloonX = 2;
      balloonY = 57;
      balloonDirection = Direction::RIGHT;
      lastMillisBalloon = millis();
    }
    specialName = name;
    scrollX = dateCanvas.width();
    scrollTextWidth = 0;
    lastMillisScroll = millis();
    drawDate();
  }
  if (!specialName.empty() && millis() - lastMillisScroll >= 40) {
    lastMillisScroll = millis();
    scrollX--;
    if (scrollX <= -scrollTextWidth) {
      scrollX = dateCanvas.width();
    }
    drawDate();
  }

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

  if (!specialName.empty()) {
    if (millis() - lastMillisBalloon >= 100) {
      restoreBalloon();
      moveBalloon();
      lastMillisBalloon = millis();
    }
    drawBalloon();
    // Keep Pac-Man visible when the two sprites cross paths.
    pacman->init();
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
    drawDate();
    
    Locator::getDisplay()->setFont(&hourFont);
    
    Locator::getDisplay()->setTextColor(0xFE40);
    Locator::getDisplay()->setCursor(15, 28);
    
    Locator::getDisplay()->print(this->_dateTime->getHour("00"));
    Locator::getDisplay()->print(" ");
    Locator::getDisplay()->print(this->_dateTime->getMinute("00"));
}

void Clockface::drawDate() {
  Adafruit_GFX* display = Locator::getDisplay();
  display->fillRect(14, 36, dateCanvas.width(), dateCanvas.height(), 0x0000);
  if (specialName.empty()) {
    display->setFont(&Picopixel);
    display->setTextColor(0xAD55);
    display->setCursor(15, 41);
    display->print(monthName(this->_dateTime->getMonth()));
    display->print(" ");
    display->print(this->_dateTime->getDay());
    display->print(" ");
    display->print(weekDayName(this->_dateTime->getWeekday()));
    display->setFont(&hourFont);
    return;
  }

  if (dateCanvas.getBuffer() == nullptr) return;
  char text[64];
  snprintf(text, sizeof(text), "Happy Birthday %s!", specialName.c_str());
  if (scrollTextWidth == 0) {
    int16_t x1, y1;
    uint16_t width, height;
    dateCanvas.getTextBounds(text, 0, 5, &x1, &y1, &width, &height);
    scrollTextWidth = width;
  }

  // Render offscreen so text cannot spill into the surrounding maze.
  dateCanvas.fillScreen(0);
  dateCanvas.setCursor(scrollX, 5);
  dateCanvas.print(text);
  display->drawBitmap(14, 36, dateCanvas.getBuffer(), dateCanvas.width(), dateCanvas.height(), 0xF81F, 0x0000);
}

void Clockface::drawBalloon() {
  ImageUtils::drawTransparent(balloonX, balloonY, _PACMAN_BALLOON, 5, 5, 0);
}

void Clockface::restoreBalloon() {
  for (int row = (balloonY - 2) / 5; row <= (balloonY + 2) / 5; row++) {
    for (int column = (balloonX - 2) / 5; column <= (balloonX + 2) / 5; column++) {
      drawMapBlock(column, row);
    }
  }
}

void Clockface::moveBalloon() {
  if ((balloonX - 2) % 5 == 0 && (balloonY - 2) % 5 == 0) {
    const Direction reverse[] = {Direction::LEFT, Direction::RIGHT, Direction::DOWN, Direction::UP};
    Direction candidates[4];
    int count = 0;
    for (int i = 0; i < 4; i++) {
      Direction direction = static_cast<Direction>(i);
      if (direction != reverse[balloonDirection] &&
          contains(nextBlock(direction, balloonX, balloonY), BALLOON_MOVING_BLOCKS)) {
        candidates[count++] = direction;
      }
    }
    if (count > 0) {
      balloonDirection = candidates[random(count)];
    } else if (contains(nextBlock(reverse[balloonDirection], balloonX, balloonY), BALLOON_MOVING_BLOCKS)) {
      balloonDirection = reverse[balloonDirection];
    } else {
      ESP_LOGW("pacman_Clockface", "Balloon has no traversable neighboring maze block");
      return;
    }
  }

  switch (balloonDirection) {
    case Direction::RIGHT: balloonX++; break;
    case Direction::LEFT: balloonX--; break;
    case Direction::UP: balloonY--; break;
    case Direction::DOWN: balloonY++; break;
  }
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
  return nextBlock(dir, pacman->getX(), pacman->getY());
}

Clockface::MapBlock Clockface::nextBlock(Direction dir, int x, int y) {
  Clockface::MapBlock map_block = Clockface::MapBlock::OUT_OF_MAP;

  if (dir == Direction::RIGHT) {
    if (x + 5 < MAP_MAX_POS) {
      map_block = static_cast<MapBlock>(_MAP[(y-2)/5][((x-2)/5)+1]);
    }
    
  } else if (dir == Direction::DOWN) {
    if (y + 5 < MAP_MAX_POS) {
      map_block = static_cast<MapBlock>(_MAP[((y-2)/5)+1][(x-2)/5]);
    }
  } else if (dir == Direction::LEFT) {

    if ((x-2) > 0) {
      map_block = static_cast<MapBlock>(_MAP[(y-2)/5][((x-2)/5)-1]);
    }

  } else if (dir == Direction::UP) {
    if ((y-2) > 0) {
      map_block = static_cast<MapBlock>(_MAP[((y-2)/5)-1][((x-2)/5)]);
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



  uint16_t wall_color = 0x0016;



  Locator::getDisplay()->drawRect(0,0,64,64,wall_color);
  Locator::getDisplay()->drawRect(1,1,62,62,wall_color);


  for (int i=0; i<MAP_SIZE; i++) {
    for (int j=0; j<MAP_SIZE; j++) {
      drawMapBlock(i, j);
      if (_MAP[j][i] == MapBlock::PACMAN) {
        pacman = new Pacman((i*5)+2,(j*5)+2);

        // Locator::getDisplay()->drawRGBBitmap((i*5)+2,(j*5)+2, _PACMAN_2, 5, 5);
        // pacmanState = !pacmanState;
      }
    }
  }

}

void Clockface::drawMapBlock(int column, int row) {
  Adafruit_GFX* display = Locator::getDisplay();
  int x = column * 5 + 2;
  int y = row * 5 + 2;
  display->fillRect(x, y, 5, 5, 0);
  switch (_MAP[row][column]) {
    case MapBlock::FOOD:
    case MapBlock::GATE:
      display->fillRect(x + 1, y + 2, 3, 1, 0xB58C);
      break;
    case MapBlock::WALL:
    case MapBlock::CLOCK:
      display->fillRect(x, y, 5, 5, 0x0016);
      break;
    case MapBlock::SUPER_FOOD:
      display->fillRect(x + 1, y + 1, 3, 3, 0xFBE0);
      break;
  }
}

} // namespace pacman
