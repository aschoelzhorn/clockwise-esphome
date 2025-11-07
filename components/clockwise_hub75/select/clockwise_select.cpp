#include "clockwise_select.h"
#include "esphome/core/log.h"

namespace esphome {
namespace clockwise_hub75 {

static const char *const TAG = "clockwise_select";

void ClockwiseSelect::setup() {
  if (parent_ != nullptr) {
    // Initialize with current clockface type from parent
    std::string current = clockface_type_to_string(parent_->get_clockface_type());
    this->publish_state(current);
  }
}

void ClockwiseSelect::dump_config() {
  ESP_LOGCONFIG(TAG, "Clockwise Clockface Select:");
  LOG_SELECT("", "Clockface", this);
}

void ClockwiseSelect::control(const std::string &value) {
  if (parent_ != nullptr) {
    ClockfaceType type = string_to_clockface_type(value);
    parent_->switch_clockface(type);
    this->publish_state(value);
    ESP_LOGD(TAG, "Setting clockface to %s", value.c_str());
  }
}

std::string ClockwiseSelect::clockface_type_to_string(ClockfaceType type) {
  switch (type) {
    case PACMAN:
      return "Pacman";
    case MARIO:
      return "Mario";
    case DUNE:
      return "Dune";
    default:
      return "Pacman";
  }
}

ClockfaceType ClockwiseSelect::string_to_clockface_type(const std::string &str) {
  if (str == "Mario") {
    return MARIO;
  } else if (str == "Pacman") {
    return PACMAN;
  } else if (str == "Dune") {
    return DUNE;
  } else {
    return PACMAN;  // Default
  }
}

}  // namespace clockwise_hub75
}  // namespace esphome