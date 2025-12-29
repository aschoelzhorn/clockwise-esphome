#include "mario_Clockface.h"
#include "mario_goomba.h"
#include "esphome/core/log.h"

const char* FORMAT_TWO_DIGITS = "%02d";

static const char *const TAG = "mario_Clockface";

// Star positions for night mode (x, y coordinates)
static const int STAR_POSITIONS[][2] = {
    {10, 18}, {55, 6}, {60, 10}, {55, 22}, {25, 1}, {45, 2}
};
static const byte STAR_COUNT = 6;

namespace mario {
Clockface::Clockface(Adafruit_GFX* display) {
  _display = display;
  
  // Initialize pointers to nullptr - objects will be created in setup()
  eventBus = nullptr;
  ground = nullptr;
  bush = nullptr;
  cloud1 = nullptr;
  cloud2 = nullptr;
  hill = nullptr;
  moon = nullptr;
  mario = nullptr;
  goomba = nullptr;
  hourBlock = nullptr;
  minuteBlock = nullptr;

  Locator::provide(display);
}

Clockface::~Clockface() {
  delete eventBus;
  delete ground;
  delete bush;
  delete cloud1;
  delete cloud2;
  delete hill;
  delete moon;
  delete mario;
  delete goomba;
  delete hourBlock;
  delete minuteBlock;
}

void Clockface::setup(CWDateTime *dateTime) {
  _dateTime = dateTime;

  // Create objects here instead of in constructor to avoid initialization order issues
  eventBus = new EventBus();
  ground = new Tile(GROUND, 8, 8);
  bush = new Object(BUSH, 21, 9);
  cloud1 = new Object(CLOUD1, 13, 12);
  cloud2 = new Object(CLOUD2, 13, 12);
  hill = new Object(HILL, 20, 22);
  moon = new Object(MOON, MOON_SIZE[0], MOON_SIZE[1]);
  mario = new Mario(23, 40);
  goomba = new Goomba(-8, 48);
  hourBlock = new Block(13, 8);
  minuteBlock = new Block(32, 8);

  // Provide EventBus after creation
  Locator::provide(eventBus);

  Locator::getDisplay()->setFont(&Super_Mario_Bros__24pt7b);

  // Check if we need to switch between day/night mode
  bool shouldBeNight = shouldBeNightMode();
  applyNightMode(shouldBeNight);
  
  drawStaticObjects();

  updateTime();

  hourBlock->init();
  minuteBlock->init();
  mario->init();
  goomba->init(_dateTime);
}

void Clockface::drawStaticObjects() {
  ground->fillRow(DISPLAY_HEIGHT - ground->_height);
  //bush->draw(43, 47);
  ImageUtils::drawTransparent(43, 47, BUSH, 21, 9, SKY_COLOR_NIGHT);
  //hill->draw(0, 34);
  ImageUtils::drawTransparent(0, 34, HILL, 20, 22, SKY_COLOR_NIGHT);
  if (!_isNightMode) {
    cloud1->draw(0, 21);
    cloud2->draw(51, 7);
  } else {
    ImageUtils::drawTransparent(3, 3, MOON, MOON_SIZE[0], MOON_SIZE[1], SKY_COLOR_NIGHT);
    //moon->draw(3, 3);
    drawStars();
  }
}

void Clockface::update() {
  
  // Check if we need to switch between day/night mode
  bool shouldBeNight = shouldBeNightMode();
  if (shouldBeNight != _isNightMode) {
    applyNightMode(shouldBeNight);
  }
  
  hourBlock->update();
  minuteBlock->update();
  mario->update();
  goomba->update();

  if (_dateTime->getSecond() == 0 && millis() - lastMillis > 1000) {
    ESP_LOGD(TAG, "Time-based jump and TIME_UPDATE broadcast");
    mario->jump(true);  // Time-based jump - should hit blocks
    updateTime();
    lastMillis = millis();
  }
}  

void Clockface::updateTime() {
  ESP_LOGD(TAG, "updateTime() called - Hour: %d, Minute: %02d", _dateTime->getHour(), _dateTime->getMinute());
  hourBlock->setText(String(_dateTime->getHour()));
  minuteBlock->setText(String(_dateTime->getMinute(FORMAT_TWO_DIGITS)));
}

void Clockface::externalEvent(int type) {
  if (type == 0) {  //TODO create an enum
    mario->jump(true);  // External event jump - should hit blocks
    updateTime();
  }
}

bool Clockface::shouldBeNightMode() {
  return true;
  // Night mode from 20:00 (8 PM) to 06:00 (6 AM)
  int hour = _dateTime->getHour();
  return (hour >= 20 || hour < 6);
}

void Clockface::applyNightMode(bool enable) {
  _isNightMode = enable;
  
  // Redraw entire background
  uint16_t skyColor = _isNightMode ? SKY_COLOR_NIGHT : SKY_COLOR;
  Locator::getDisplay()->fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, skyColor);
  
  drawStaticObjects();
    
  ESP_LOGD(TAG, "Switched to %s mode", _isNightMode ? "night" : "day");
}

void Clockface::drawStars() {
  // Draw stars using STAR sprite from mario_assets
  for (int i = 0; i < STAR_COUNT; i++) {
    int x = STAR_POSITIONS[i][0] - 1;  // Center the 3x3 sprite
    int y = STAR_POSITIONS[i][1] - 1;
    //Locator::getDisplay()->drawRGBBitmap(x, y, STAR, STAR_SIZE[0], STAR_SIZE[1]);
    ImageUtils::drawTransparent(x, y, STAR, STAR_SIZE[0], STAR_SIZE[1], SKY_COLOR_NIGHT);
  }
}

// void Clockface::drawTransparent(int x, int y, const uint16_t* bitmap, int width, int height, uint16_t maskColor) {
//   for (int j = 0; j < height; j++) {
//     for (int i = 0; i < width; i++) {
//       uint16_t pixel = pgm_read_word(&bitmap[j * width + i]);
//       if (pixel != MASK_COLOR) {
//         Locator::getDisplay()->drawPixel(x + i, y + j, pixel);
//       }
//     }
//   }
// }

}  // namespace mario
