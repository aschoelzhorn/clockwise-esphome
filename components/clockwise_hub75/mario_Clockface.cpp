#include "mario_Clockface.h"
#include "mario_goomba.h"
#include "esphome/core/log.h"

const char* FORMAT_TWO_DIGITS = "%02d";

static const char *const TAG = "mario_Clockface";

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
  mario = new Mario(23, 40);
  goomba = new Goomba(-8, 48);
  hourBlock = new Block(13, 8);
  minuteBlock = new Block(32, 8);

  // Provide EventBus after creation
  Locator::provide(eventBus);

  Locator::getDisplay()->setFont(&Super_Mario_Bros__24pt7b);
  Locator::getDisplay()->fillRect(0, 0, 64, 64, SKY_COLOR);

  ground->fillRow(DISPLAY_HEIGHT - ground->_height);

  bush->draw(43, 47);
  hill->draw(0, 34);
  cloud1->draw(0, 21);
  cloud2->draw(51, 7);

  updateTime();

  hourBlock->init();
  minuteBlock->init();
  mario->init();
  goomba->init(_dateTime);
}

void Clockface::update() {
  
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

}  // namespace mario
