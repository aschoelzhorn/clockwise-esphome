
#include "CWDateTime.h"
#include "clockwise_hub75.h"
#include "esphome/core/log.h"
#include "Locator.h"
#include "GFXWrapper.h"
#include "pacman_Clockface.h"
#include "mario_Clockface.h"
#include <cctype>
#include <cstring>

::CWDateTime g_dt;

namespace esphome {
namespace clockwise_hub75 {

namespace {

std::string trim_copy(const std::string &in) {
  size_t start = 0;
  while (start < in.size() && std::isspace(static_cast<unsigned char>(in[start]))) {
    start++;
  }
  size_t end = in.size();
  while (end > start && std::isspace(static_cast<unsigned char>(in[end - 1]))) {
    end--;
  }
  return in.substr(start, end - start);
}

bool parse_int_in_range(const std::string &s, int min_value, int max_value, int &out) {
  if (s.empty()) {
    return false;
  }
  for (char ch : s) {
    if (!std::isdigit(static_cast<unsigned char>(ch))) {
      return false;
    }
  }
  int value = std::atoi(s.c_str());
  if (value < min_value || value > max_value) {
    return false;
  }
  out = value;
  return true;
}

}  // namespace

static const char *const TAG = "clockwise_hub75";

void ClockwiseHUB75::set_time_source(int source) {
  // 0 = Home Assistant, 1 = NTP (SNTP), 2 = RTC
  time_source_ = source;
  ESP_LOGD(TAG, "set_time_source: source=%d ha=%p ntp=%p rtc=%p", source, ha_time_, ntp_time_, rtc_time_);

  if (source == 0 && ha_time_ != nullptr) {
    ESP_LOGI(TAG, "Switching to Home Assistant time");
    set_time(ha_time_);
  } else if (source == 1 && ntp_time_ != nullptr) {
    ESP_LOGI(TAG, "Switching to NTP time");
    set_time(ntp_time_);
  } else if (source == 2 && rtc_time_ != nullptr) {
    ESP_LOGI(TAG, "Switching to RTC time");
    set_time(rtc_time_);
  }
  
  if (time_ != nullptr) {
    ESP_LOGD(TAG, "g_dt.set_rtc: time_=%p", time_);
    g_dt.set_rtc(time_);
  }
}

static IClockface *create_clockface_(ClockfaceType type, GFXWrapper *gfx) {
  switch (type) {
    case PACMAN:
      return new pacman::Clockface(gfx);
    case MARIO:
      return new mario::Clockface(gfx);
    default:
      return nullptr;
  }
}

void ClockwiseHUB75::set_panel_color_order(PanelColorOrder order) {
  panel_color_order_ = order;

  if (gfx_wrapper_ != nullptr) {
    gfx_wrapper_->set_color_order(static_cast<GFXWrapper::ColorChannelOrder>(order));
  }

  // Force full redraw for clockfaces that keep static pixels on screen (e.g. Pacman/Mario).
  if (clockface_ != nullptr && gfx_wrapper_ != nullptr) {
    switch_clockface(clockface_type_, true);
  }

  ESP_LOGI(TAG, "Panel color order set to %d", static_cast<int>(order));
}

void ClockwiseHUB75::set_special_dates(const std::string &value) {
  special_dates_csv_ = value;
  parse_special_dates_(value);
  ESP_LOGI(TAG, "Special dates updated, %u valid entries", static_cast<unsigned int>(special_date_count_));
}

void ClockwiseHUB75::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Clockwise HUB75 Integration...");
  
  if (hub75_display_ == nullptr) {
    ESP_LOGE(TAG, "HUB75 display pointer not set (codegen failed to inject). Aborting setup.");
    return;
  }
  
  // Set initial brightness
  current_brightness_ = initial_brightness_;
  hub75_display_->set_brightness(current_brightness_);
  hub75_display_->fill(Color(0,0,0));
  
  // Create Adafruit_GFX wrapper around HUB75Display
  gfx_wrapper_ = new GFXWrapper(hub75_display_);
  gfx_wrapper_->set_color_order(static_cast<GFXWrapper::ColorChannelOrder>(panel_color_order_));
  
  // Provide the wrapper to the Clockface engine via Locator
  Locator::provide(gfx_wrapper_);
  
  // Initialize datetime shim and wire initial time source
  g_dt.begin();
  set_time_source(time_source_);

  // Create initial clockface
  clockface_ = create_clockface_(clockface_type_, gfx_wrapper_);
  if (clockface_ != nullptr) {
    clockface_->setup(&g_dt);
    ESP_LOGCONFIG(TAG, "Clockface %d initialized", static_cast<int>(clockface_type_));
  } else {
    ESP_LOGW(TAG, "Clockface %d not yet implemented", static_cast<int>(clockface_type_));
  }
  
  ESP_LOGCONFIG(TAG, "Clockwise HUB75 setup complete");
}

void ClockwiseHUB75::dump_config() {
  ESP_LOGCONFIG(TAG, "Clockwise HUB75:");
  ESP_LOGCONFIG(TAG, "  Clockface Type: %d", static_cast<int>(clockface_type_));
  ESP_LOGCONFIG(TAG, "  Panel Color Order: %d", static_cast<int>(panel_color_order_));
  ESP_LOGCONFIG(TAG, "  Initial Brightness: %d", initial_brightness_);
  ESP_LOGCONFIG(TAG, "  Current Brightness: %d", current_brightness_);
  ESP_LOGCONFIG(TAG, "  Power State: %s", YESNO(power_state_));
  ESP_LOGCONFIG(TAG, "  Special Dates: %s", special_dates_csv_.c_str());
  ESP_LOGCONFIG(TAG, "  Special Date Entries: %u", static_cast<unsigned int>(special_date_count_));
}

void ClockwiseHUB75::set_brightness(uint8_t brightness) {
  current_brightness_ = brightness;
  if (hub75_display_ != nullptr) {
    hub75_display_->set_brightness(brightness);
  }
  ESP_LOGD(TAG, "Brightness set to %d", brightness);
}

void ClockwiseHUB75::set_power(bool state) {
  power_state_ = state;
  if (hub75_display_ != nullptr) {
    if (state) {
      hub75_display_->set_brightness(current_brightness_);
    } else {
      hub75_display_->set_brightness(0);
    }
  }
  ESP_LOGD(TAG, "Power state set to %s", YESNO(state));
}

void ClockwiseHUB75::switch_clockface(ClockfaceType type, bool force) {
  if (!force && type == clockface_type_) {
    ESP_LOGD(TAG, "Already using clockface type %d, skipping switch", static_cast<int>(type));
    return;
  }

  // Called before setup() (e.g. from a template select restoring its value):
  // gfx_wrapper_ isn't created yet, so just record the type and return.
  // setup() will then create the correct clockface.
  if (gfx_wrapper_ == nullptr) {
    ESP_LOGD(TAG, "switch_clockface called before setup, storing type %d", static_cast<int>(type));
    clockface_type_ = type;
    return;
  }

  ESP_LOGI(TAG, "Switching clockface from %d to %d", static_cast<int>(clockface_type_), static_cast<int>(type));

  // Destroy the old clockface
  if (clockface_ != nullptr) {
    delete clockface_;
    clockface_ = nullptr;
  }
  
  // Clear the display
  if (hub75_display_ != nullptr) {
    hub75_display_->fill(Color(0, 0, 0));
  }
  
  // Update the clockface type
  clockface_type_ = type;

  clockface_ = create_clockface_(clockface_type_, gfx_wrapper_);
  if (clockface_ != nullptr) {
    clockface_->setup(&g_dt);
    ESP_LOGI(TAG, "Switched to clockface %d", static_cast<int>(clockface_type_));
  } else {
    ESP_LOGW(TAG, "Clockface %d not yet implemented", static_cast<int>(clockface_type_));
  }
}

void ClockwiseHUB75::update_display_() {
  if (!power_state_ || clockface_ == nullptr) {
    return;
  }
  // Call IClockface update
  clockface_->update();
}

void ClockwiseHUB75::parse_special_dates_(const std::string &value) {
  special_date_count_ = 0;
  for (auto &entry : special_dates_) {
    entry = SpecialDateEntry{};
  }

  if (value.empty()) {
    return;
  }

  size_t token_start = 0;
  while (token_start < value.size() && special_date_count_ < MAX_SPECIAL_DATES) {
    size_t token_end = value.find(',', token_start);
    if (token_end == std::string::npos) {
      token_end = value.size();
    }

    std::string token = trim_copy(value.substr(token_start, token_end - token_start));
    if (!token.empty()) {
      size_t colon_pos = token.find(':');
      if (colon_pos == std::string::npos) {
        ESP_LOGW(TAG, "Skipping invalid special date token (missing ':'): %s", token.c_str());
      } else {
        std::string date_part = trim_copy(token.substr(0, colon_pos));
        std::string name_part = trim_copy(token.substr(colon_pos + 1));

        size_t sep_pos = date_part.find('-');
        if (sep_pos == std::string::npos) {
          sep_pos = date_part.find('/');
        }

        if (sep_pos == std::string::npos || name_part.empty()) {
          ESP_LOGW(TAG, "Skipping invalid special date token: %s", token.c_str());
        } else {
          int month = 0;
          int day = 0;
          const std::string month_str = trim_copy(date_part.substr(0, sep_pos));
          const std::string day_str = trim_copy(date_part.substr(sep_pos + 1));

          if (!parse_int_in_range(month_str, 1, 12, month) || !parse_int_in_range(day_str, 1, 31, day)) {
            ESP_LOGW(TAG, "Skipping special date with invalid month/day: %s", token.c_str());
          } else {
            SpecialDateEntry &entry = special_dates_[special_date_count_++];
            entry.month = static_cast<uint8_t>(month);
            entry.day = static_cast<uint8_t>(day);
            std::strncpy(entry.name, name_part.c_str(), MAX_SPECIAL_NAME_LEN);
            entry.name[MAX_SPECIAL_NAME_LEN] = '\0';
            entry.used = true;
          }
        }
      }
    }

    token_start = token_end + 1;
  }
}

bool ClockwiseHUB75::get_today_special_name_(std::string &name_out) const {
  const int month = g_dt.getMonth();
  const int day = g_dt.getDay();
  if (month <= 0 || day <= 0) {
    return false;
  }

  for (size_t i = 0; i < special_date_count_; i++) {
    const SpecialDateEntry &entry = special_dates_[i];
    if (!entry.used) {
      continue;
    }
    if (entry.month == static_cast<uint8_t>(month) && entry.day == static_cast<uint8_t>(day)) {
      name_out = entry.name;
      return true;
    }
  }
  return false;
}

bool ClockwiseHUB75::is_today_special_date() const {
  std::string name;
  return get_today_special_name_(name);
}

std::string ClockwiseHUB75::get_today_special_name() const {
  std::string name;
  if (get_today_special_name_(name)) {
    return name;
  }
  return "";
}

}  // namespace clockwise_hub75
}  // namespace esphome