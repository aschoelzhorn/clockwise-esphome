#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>

#include "pacman_hour_font.h"
#include "picopixel.h"
#include "Tile.h"
#include "Locator.h"
#include "Game.h"
#include "Object.h"
#include "ImageUtils.h"
#include "ColorUtil.h"
#include "IClockface.h"

//sprites
#include "pacman.h"


namespace pacman {
class Clockface: public IClockface {
  private:
    const int MAP_SIZE = 12;
    Adafruit_GFX* _display;
    CWDateTime* _dateTime;
    bool pacmanState = true;
    bool show_seconds = true;

    const char* _weekDayWords = "SUN\0MON\0TUE\0WED\0THU\0FRI\0SAT\0";
    const char* _monthWords = "JAN\0FEB\0MAR\0APR\0MAY\0JUN\0JUL\0AUG\0SEP\0OCT\0NOV\0DEC\0";
    char weekDayTemp[4]= "\0";
    char monthTemp[4]= "\0";

    // Timing and sprite state
    unsigned long lastMillis = 0;
    unsigned long lastMillisTime = 0;
    unsigned long lastMillisSec = 0;
    Pacman* pacman = nullptr;



    enum MapBlock {
      EMPTY = 0,
      FOOD = 1,
      WALL = 2,
      GATE = 3,
      SUPER_FOOD = 4,
      CLOCK = 5,
      GHOST = 6,
      PACMAN = 7,
      OUT_OF_MAP = 99
    };


    const byte _MAP_CONST[12][12] = {
      {4,1,1,1,1,1,7,1,1,1,1,4},
      {1,2,2,1,2,2,2,2,1,2,2,1},
      {1,1,1,1,1,2,2,1,1,1,1,1},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {3,1,5,5,5,5,5,5,5,5,1,3},
      {3,1,5,5,5,5,5,5,5,5,1,3},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {1,1,1,1,1,2,2,1,1,1,1,1},
      {1,2,2,1,2,2,2,2,1,2,2,1},
      {4,1,1,1,1,1,1,1,1,1,1,4}
    };

    
    byte _MAP[12][12] = {
      {4,1,1,1,1,1,7,1,1,1,1,4},
      {1,2,2,1,2,2,2,2,1,2,2,1},
      {1,1,1,1,1,2,2,1,1,1,1,1},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {3,1,5,5,5,5,5,5,5,5,1,3},
      {3,1,5,5,5,5,5,5,5,5,1,3},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {2,1,5,5,5,5,5,5,5,5,1,2},
      {1,1,1,1,1,2,2,1,1,1,1,1},
      {1,2,2,1,2,2,2,2,1,2,2,1},
      {4,1,1,1,1,1,1,1,1,1,1,4}
    };


    const byte MAP_BORDER_SIZE = 2;
    const byte MAP_MIN_POS = 0 + MAP_BORDER_SIZE;
    const byte MAP_MAX_POS = 64 - MAP_BORDER_SIZE;

    // first elem is the size
    const int PACMAN_MOVING_BLOCKS[4] = {3, MapBlock::EMPTY, MapBlock::FOOD, MapBlock::GATE};
    const int PACMAN_BLOCKING_BLOCKS[4] = {3, MapBlock::OUT_OF_MAP, MapBlock::WALL, MapBlock::CLOCK};

    void drawMap();
    Clockface::MapBlock nextBlock(Direction dir);
    Clockface::MapBlock nextBlock();
    void turnRandom();
    int countBlocks(Clockface::MapBlock elem);
    bool contains(int v, const int* values);
    void resetMap();
    void directionDecision(MapBlock nextBlk, bool moving_axis_x);
    void updateClock();
    const char* weekDayName(int weekday);
    const char* monthName(int month);
    
    
  public:
    Clockface(Adafruit_GFX* display);
    ~Clockface();
    void setup(CWDateTime *dateTime);
    void update();
};
}  // namespace pacman
