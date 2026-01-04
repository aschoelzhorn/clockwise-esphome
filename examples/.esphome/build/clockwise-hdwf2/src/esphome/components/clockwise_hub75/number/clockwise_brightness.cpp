#include "clockwise_brightness.h"
#include "esphome/core/log.h"

namespace esphome {
namespace clockwise_hub75 {

static const char *const TAG = "clockwise_brightness";

void ClockwiseBrightness::setup() {
  if (parent_ != nullptr) {
    // Initialize with current brightness from parent
    this->publish_state(parent_->get_brightness());
  }
}

void ClockwiseBrightness::dump_config() {
  ESP_LOGCONFIG(TAG, "Clockwise Brightness Number:");
  LOG_NUMBER("", "Brightness", this);
}

void ClockwiseBrightness::control(float value) {
  if (parent_ != nullptr) {
    uint8_t brightness = static_cast<uint8_t>(value);
    parent_->set_brightness(brightness);
    this->publish_state(value);
    ESP_LOGD(TAG, "Setting brightness to %.0f", value);
  }
}

}  // namespace clockwise_hub75
}  // namespace esphome