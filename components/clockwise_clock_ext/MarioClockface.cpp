#include "MarioClockface.h"
#include "goomba.h"
#include "esphome/core/log.h"

static const char *TAG = "mario_clockface";

const char* FORMAT_TWO_DIGITS = "%02d";

MarioClockface::MarioClockface(Adafruit_GFX* display) {
  _display = display;

  // Initialize member objects with proper pointers
  eventBus = new EventBus();
  ground = new Tile(GROUND, 8, 8);
  bush = new Object(BUSH, 21, 9);
  cloud1 = new Object(CLOUD1, 13, 12);
  cloud2 = new Object(CLOUD2, 13, 12);
  hill = new Object(HILL, 20, 22);
  mario = new Mario(23, 40);
  goomba = new Goomba(-8, 48);  // Start outside left edge (width is 8px)
  hourBlock = new Block(13, 8);
  minuteBlock = new Block(32, 8);

  Locator::provide(display);
  Locator::provide(eventBus);
}

MarioClockface::~MarioClockface() {
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

void MarioClockface::setup(CWDateTime *dateTime) {
  _dateTime = dateTime;

  // Subscribe to events to handle time updates
  Locator::getEventBus()->subscribe(this);

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
  goomba->init();
}

void MarioClockface::update() {
  hourBlock->update();
  minuteBlock->update();
  mario->update();
  goomba->update();

  if (_dateTime->getSecond() == 0 && millis() - lastMillis > 1000) {
    ESP_LOGD(TAG, "Time-based jump and TIME_UPDATE broadcast");
    mario->jump();  // Just jump, don't update time directly
    Locator::getEventBus()->broadcast(TIME_UPDATE, nullptr);  // Broadcast time update event
    lastMillis = millis();

    //Serial.println(_dateTime->getFormattedTime());
  }
}  

void MarioClockface::updateTime() {
  ESP_LOGD(TAG, "updateTime() called - Hour: %d, Minute: %02d", _dateTime->getHour(), _dateTime->getMinute());
  
  hourBlock->setText(String(_dateTime->getHour()));
  minuteBlock->setText(String(_dateTime->getMinute(FORMAT_TWO_DIGITS)));
}

void MarioClockface::externalEvent(int type) {
  if (type == 0) {  //TODO create an enum
    mario->jump();  // Just jump, don't update time directly
    Locator::getEventBus()->broadcast(TIME_UPDATE, nullptr);  // Broadcast time update event
  }  
}

void MarioClockface::execute(EventType event, Sprite* caller) {
  if (event == EventType::TIME_UPDATE) {
    ESP_LOGD(TAG, "TIME_UPDATE event received - updating time blocks");
    updateTime();  // Only update time blocks when TIME_UPDATE event is received
  }
}

const char* MarioClockface::name() {
  return "MARIO_CLOCKFACE";
}
