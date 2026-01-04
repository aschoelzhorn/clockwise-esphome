#include "clockwise_switch.h"
#include "esphome/core/log.h"

namespace esphome {
namespace clockwise_hub75 {

static const char *const TAG = "clockwise_switch";

void ClockwiseSwitch::setup() {
  if (parent_ != nullptr) {
    // Initialize with current power state from parent
    this->publish_state(parent_->get_power());
  }
}

void ClockwiseSwitch::dump_config() {
  ESP_LOGCONFIG(TAG, "Clockwise Power Switch:");
  LOG_SWITCH("", "Power", this);
}

void ClockwiseSwitch::write_state(bool state) {
  if (parent_ != nullptr) {
    parent_->set_power(state);
    this->publish_state(state);
    ESP_LOGD(TAG, "Setting power state to %s", YESNO(state));
  }
}

}  // namespace clockwise_hub75
}  // namespace esphome