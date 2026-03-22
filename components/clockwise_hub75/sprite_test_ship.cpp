#include "sprite_test_ship.h"
#include "esphome/core/log.h"

static const char *const TAG = "TestShip";

namespace story {

TestShip::TestShip() : Sprite(), EventTask() {
  // Initialize sprite to GOOMBA from mario assets
  _sprite = GOOMBA;
  _x = -8;  // Start off-screen to the left
  _y = 20;  // Vertical position (middle-ish of display)
  _width = 8;
  _height = 8;
  _active = true;
}

TestShip::~TestShip() {
}

const char* TestShip::name() {
  return "TestShip";
}

void TestShip::execute(EventType event, Sprite* caller, uint16_t value) {
  // Handle event-based actions if needed
  ESP_LOGD(TAG, "TestShip received event: %d", event);
}

void TestShip::update() {
  if (!_active) {
    return;
  }

  uint32_t now = millis();
  if (now - _lastUpdate < _updateIntervalMs) {
    return;  // Not time to update yet
  }

  _lastUpdate = now;

  // Move right
  _x += _pace;

  // Wrap around - when reaching the right side, start from left again
  if (_x > DISPLAY_WIDTH) {
    _x = -_width;
  }

  ESP_LOGD(TAG, "Update: x=%d, y=%d", _x, _y);
}

}  // namespace story
