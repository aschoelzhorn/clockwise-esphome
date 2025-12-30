#include "mario_Clockface.h"
#include "mario_enemy.h"
#include "esphome/core/log.h"

const char* FORMAT_TWO_DIGITS = "%02d";

static const char *const TAG = "mario_Clockface";

// Star positions for night mode (x, y coordinates of center of star)
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
  for (int i = 0; i < ENEMY_COUNT; ++i) enemies[i] = nullptr;
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
  for (int i = 0; i < ENEMY_COUNT; ++i) delete enemies[i];
  delete hourBlock;
  delete minuteBlock;
}

void Clockface::setup(CWDateTime *dateTime) {
  _dateTime = dateTime;

  // Create objects here instead of in constructor to avoid initialization order issues
  eventBus = new EventBus();
  ground = new Tile(GROUND, GROUND_SIZE[0], GROUND_SIZE[1]);
  bush = new Object(BUSH, BUSH_SIZE[0], BUSH_SIZE[1]);
  cloud1 = new Object(CLOUD1, CLOUD1_SIZE[0], CLOUD1_SIZE[1]);
  cloud2 = new Object(CLOUD2, CLOUD2_SIZE[0], CLOUD2_SIZE[1]);
  hill = new Object(HILL, HILL_SIZE[0], HILL_SIZE[1]);
  moon = new Object(MOON, MOON_SIZE[0], MOON_SIZE[1]);
  mario = new Mario(23, 40);
  hourBlock = new Block(13, 8);
  minuteBlock = new Block(32, 8);
 
  // Goomba
  enemies[0] = new Enemy(-ENEMY_SIZE[0], 48, EnemyType::GOOMBA);
  // Koopa
  enemies[1] = new Enemy(-ENEMY_SIZE[0], 48, EnemyType::KOOPA);
  
  _activeEnemyIdx = -1; // no active enemy at start
  _nextEnemyRunTime = 0; // don't start at all until scheduled

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
  for (int i = 0; i < ENEMY_COUNT; ++i) enemies[i]->init();
}

void Clockface::drawStaticObjects() {
  ground->fillRow(DISPLAY_HEIGHT - ground->_height);
  bush->drawTransparent(43, 47, _skyColor);
  hill->drawTransparent(0, 34, _skyColor);
  
  if (!_isNightMode) {
    cloud1->draw(0, 21);
    cloud2->draw(51, 7);
  } else {
    moon->drawTransparent(3, 3, _skyColor);
    drawStars();
  }
}

void Clockface::update() {
  
  // Check if we need to switch between day/night mode
  bool shouldBeNight = shouldBeNightMode();
  //if (shouldBeNight != _isNightMode) {
    applyNightMode(shouldBeNight);
  //}
  
  hourBlock->update();
  minuteBlock->update();
  mario->update();

  scheduleNextEnemyRun();
  if (isTimeForEnemyRun()) {
    chooseRandomEnemy();
    enemies[_activeEnemyIdx]->startRandomRun();
    _nextEnemyRunTime = 0; // Reset to indicate no run scheduled
  }

  enemies[_activeEnemyIdx]->update();

  if (_dateTime->getSecond() == 0 && millis() - lastMillis > 1000) {
    ESP_LOGD(TAG, "Time-based jump and TIME_UPDATE broadcast");
    mario->jump(true);  // Time-based jump - should hit blocks
    updateTime();
    lastMillis = millis();
  }
}

bool Clockface::isTimeForEnemyRun() {
  if (_nextEnemyRunTime > 0 && millis() >= _nextEnemyRunTime) {
    if (isNearTimeChange()) {
      // Postpone if near time change
      _nextEnemyRunTime += 15000; // Postpone by 15 seconds (max time change window)
      ESP_LOGD(TAG, "Postponing enemy run due to time change proximity");
      return false;
    }
    else {
      ESP_LOGD(TAG, "It's time for enemy run!");
      return true;
    }
  }
  else {
    return false;
  }
}

bool Clockface::areAllEnemiesHidden() {
  for (int i = 0; i < ENEMY_COUNT; ++i) {
    if (!enemies[i]->isHidden()) {
      return false;
    }
  }
  return true;
} 

void Clockface::scheduleNextEnemyRun() {
  if (_nextEnemyRunTime > 0) {
    return; // Already scheduled
  } 
  if (areAllEnemiesHidden()) { 
    // Schedule next enemy run between 20 to 60 seconds from now
    unsigned long interval = random(20000, 60000);
    _nextEnemyRunTime = millis() + interval;
    ESP_LOGD(TAG, "Next enemy run scheduled in %lu ms", interval);
  }
}

bool Clockface::isNearTimeChange() {
  // Check if we're within 10 seconds before or 5 seconds after a minute change
  // This avoids interfering with Mario's time jump
  int seconds = _dateTime->getSecond();
  // Avoid 50-59 seconds (before minute change) and 0-5 seconds (after minute change)
  return (seconds >= 50 || seconds <= 5);
}


void Clockface::chooseRandomEnemy() {
  _activeEnemyIdx = random(0, ENEMY_COUNT);
}

void Clockface::updateTime() {
  ESP_LOGD(TAG, "updateTime() called - Hour: %d, Minute: %02d", _dateTime->getHour(), _dateTime->getMinute());
  hourBlock->setText(String(_dateTime->getHour()));
  minuteBlock->setText(String(_dateTime->getMinute(FORMAT_TWO_DIGITS)));
}

// Handle external events (probably not used at the moment)
void Clockface::externalEvent(int type) {
  if (type == 0) {  //TODO create an enum
    mario->jump(true);  // External event jump - should hit blocks
    updateTime();
  }
}

bool Clockface::shouldBeNightMode() {
  // Night mode from 20:00 (8 PM) to 06:00 (6 AM)
  int hour = _dateTime->getHour();
  return (hour >= 20 || hour < 6);
}

void Clockface::applyNightMode(bool enable) {
  _isNightMode = enable;
  _skyColor = _isNightMode ? SKY_COLOR_NIGHT : SKY_COLOR;
  ESP_LOGD(TAG, "_isNightMode changed to: %s. Changed _skyColor to: %u", _isNightMode ? "true" : "false", _skyColor);
  // Broadcast new sky color to all listeners
  if (eventBus) {
    ESP_LOGD(TAG, "Broadcasting SKY_COLOR_CHANGED event with color: %u", _skyColor);
    eventBus->broadcast(SKY_COLOR_CHANGED, nullptr, _skyColor);
  }

  // Redraw entire background
  Locator::getDisplay()->fillRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, _skyColor);
  
  drawStaticObjects();
    
  ESP_LOGD(TAG, "Switched to %s mode", _isNightMode ? "night" : "day");
}

void Clockface::drawStars() {
  // Draw stars using STAR sprite from mario_assets
  for (int i = 0; i < STAR_COUNT; i++) {
    int x = STAR_POSITIONS[i][0] - 1;  // Center the 3x3 sprite
    int y = STAR_POSITIONS[i][1] - 1;
    ImageUtils::drawTransparent(x, y, STAR, STAR_SIZE[0], STAR_SIZE[1], _skyColor);
  }
}

}  // namespace mario
