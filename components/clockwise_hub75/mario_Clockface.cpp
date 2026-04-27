#include "mario_Clockface.h"
#include "mario_enemy.h"
#include "SpecialDates.h"
#include "esphome/core/log.h"

const char* FORMAT_TWO_DIGITS = "%02d";

static const char *const TAG = "mario_Clockface";

// Star positions for night mode (x, y coordinates of center of star)
static const int STAR_POSITIONS[][2] = {
    {10, 18}, {55, 6}, {60, 10}, {25, 1}, {45, 2}, {55, 22}
};
static const byte STAR_COUNT = 6;

namespace mario {
Clockface::Clockface(Adafruit_GFX* display) {
  _display = display;
  
  // Initialize pointers to nullptr - objects will be created in setup()
  eventBus = nullptr;
  ground = nullptr;
  pipe = nullptr;
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
  delete pipe;
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
  pipe = new Tile(PIPE, PIPE_SIZE[0], PIPE_SIZE[1]);
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

  // Initialize scrolling text state
  _scrollX = DISPLAY_WIDTH;
  _scrollTextWidth = 0;
  _scrollLastUpdate = 0;

  // Provide EventBus after creation
  Locator::provide(eventBus);

  Locator::getDisplay()->setFont(&Super_Mario_Bros__24pt7b);

  hourBlock->init();
  minuteBlock->init();
  mario->init();
  for (int i = 0; i < ENEMY_COUNT; ++i) enemies[i]->init();

  updateTime();
}

void Clockface::drawStaticObjects() {
  bush->drawTransparent(43, 47, _skyColor);
  hill->drawTransparent(0, 34, _skyColor);
  
  if (!_isNightMode) {
    cloud1->drawTransparent(0, 21, _skyColor);
    cloud2->drawTransparent(51, 7, _skyColor);
  } else {
    moon->drawTransparent(3, 3, _skyColor);
    drawStars();
  }

  if (special_date_is_today()) {
    ImageUtils::drawTransparent(48, 21, BALLOON, BALLOON_SIZE[0], BALLOON_SIZE[1], _skyColor);
    pipe->fillRow(DISPLAY_HEIGHT - pipe->_height);
  }
  else {
    //ground->fillRow(DISPLAY_HEIGHT - ground->_height);
    pipe->fillRow(DISPLAY_HEIGHT - pipe->_height);
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

  if (special_date_is_today()) {
    drawScrollText();
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
  // Draw stars using STAR sprite from mario_assets.
  // Skip the star at {55, 22} when the balloon is shown — it overlaps the balloon position.
  bool skipOverlapStar = special_date_is_today();
  for (int i = 0; i < STAR_COUNT; i++) {
    if (skipOverlapStar && STAR_POSITIONS[i][0] == 55 && STAR_POSITIONS[i][1] == 22) continue;
    int x = STAR_POSITIONS[i][0] - 1;  // Center the 3x3 sprite
    int y = STAR_POSITIONS[i][1] - 1;
    ImageUtils::drawTransparent(x, y, STAR, STAR_SIZE[0], STAR_SIZE[1], _skyColor);
  }
}

void Clockface::drawScrollText() {
  const char* name = special_date_today_name();
  if (!name || name[0] == '\0') return;

  char text[64];
  snprintf(text, sizeof(text), "Happy birthday %s!", name);

  Adafruit_GFX* display = Locator::getDisplay();
  const int groundY = DISPLAY_HEIGHT - pipe->_height; // y = 56
  // Picopixel glyphs have yOffset = -4 and height = 5, so baseline at groundY+6
  // places characters at rows 58-62, centered within the 8-pixel ground strip.
  const int textBaselineY = groundY + 5;

  // Compute text pixel width once (requires font to be set first)
  if (_scrollTextWidth == 0) {
    display->setFont(&Picopixel);
    int16_t x1, y1;
    uint16_t w, h;
    display->getTextBounds(text, 0, textBaselineY, &x1, &y1, &w, &h);
    _scrollTextWidth = (int)w;
    display->setFont(&Super_Mario_Bros__24pt7b);
  }

  // Advance scroll position at ~25 pixels/second (40 ms per pixel)
  unsigned long now = millis();
  if (now - _scrollLastUpdate >= 40) {
    _scrollLastUpdate = now;
    _scrollX--;
    if (_scrollX <= -_scrollTextWidth) {
      _scrollX = DISPLAY_WIDTH;
    }
  }

  // Redraw ground row to erase previous text position
  pipe->fillRow(groundY);

  // Draw scrolling text over the ground
  display->setFont(&Picopixel);
  display->setTextColor(0x0008);
  display->setCursor(_scrollX, textBaselineY);
  display->print(text);
  display->setFont(&Super_Mario_Bros__24pt7b); // restore Mario font
}

}  // namespace mario
