#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>

#include "Tile.h"
#include "Locator.h"
#include "Game.h"
#include "Object.h"
#include "ImageUtils.h"
// Commons
#include "IClockface.h"
#include "CWDateTime.h"

#include "dune_assets.h"

namespace dune {
class Clockface : public IClockface {
  private:
    Adafruit_GFX* _display;
    CWDateTime* _dateTime;
    
    // Timing
    unsigned long lastMillis = 0;
    unsigned long lastMillisTime = 0;
    
    void updateTime();
    void drawBackground();

  public:
    Clockface(Adafruit_GFX* display);
    ~Clockface();
    void setup(CWDateTime *dateTime);
    void update();
};
}  // namespace dune
