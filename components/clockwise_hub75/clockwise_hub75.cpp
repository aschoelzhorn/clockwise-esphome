
#include "CWDateTime.h"
#include "clockwise_hub75.h"
#include "esphome/core/log.h"
#include "Locator.h"
#include "GFXWrapper.h"
#include "pacman_Clockface.h"
#include "mario_Clockface.h"

::CWDateTime g_dt;

namespace esphome {
namespace clockwise_hub75 {

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
  ESP_LOGCONFIG(TAG, "  Initial Brightness: %d", initial_brightness_);
  ESP_LOGCONFIG(TAG, "  Current Brightness: %d", current_brightness_);
  ESP_LOGCONFIG(TAG, "  Power State: %s", YESNO(power_state_));
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

void ClockwiseHUB75::switch_clockface(ClockfaceType type) {
  if (type == clockface_type_) {
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

}  // namespace clockwise_hub75
}  // namespace esphome