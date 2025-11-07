#pragma once

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

    void updateTime();

    // Timing
    unsigned long lastMillis = 0;
    unsigned long lastMillisTime = 0;
    unsigned long lastBackgroundChange = 0;
    
    // Background cycling
    int currentBackgroundIndex = 0;
    static const int BACKGROUND_COUNT = 5;
    static const unsigned long BACKGROUND_CHANGE_INTERVAL = 5000; // 5 seconds
    
    void drawBackground();
    void cycleBackground();

public:
    Clockface(Adafruit_GFX* display);
    ~Clockface();
    void setup(CWDateTime *dateTime);
    void update();
};
}  // namespace dune
