
#include "CWDateTime.h"
#include "clockwise_hub75.h"
#include "esphome/core/log.h"
#include "Locator.h"
#include "GFXWrapper.h"
#include "pacman_Clockface.h"  // Pacman clockface implementation
#include "mario_Clockface.h"  // For Mario face
#include "dune_Clockface.h"   // For Dune face (legacy)
#include "StoryClockface.h"   // Generic story clockface orchestrator
#include "dune_theme.h"       // New Dune theme

::CWDateTime g_dt;

namespace esphome {
namespace clockwise_hub75 {

void ClockwiseHUB75::set_ha_time(time::RealTimeClock *time) {
  ha_time_ = time;
}

void ClockwiseHUB75::set_rtc_time(time::RealTimeClock *time) {
  rtc_time_ = time;
}

void ClockwiseHUB75::set_time_source(int source) {
  // 0 = Home Assistant, 1 = RTC
  time_source_ = source;
  ESP_LOGI("clockwise_hub75", "set_time_source called: source=%d", source);
  ESP_LOGI("clockwise_hub75", "ha_time_=%p, rtc_time_=%p", ha_time_, rtc_time_);
  if (source == 0 && ha_time_ != nullptr) {
    ESP_LOGI("clockwise_hub75", "Switching to Home Assistant time");
    set_time(ha_time_);
  } else if (source == 1 && rtc_time_ != nullptr) {
    ESP_LOGI("clockwise_hub75", "Switching to RTC time");
    set_time(rtc_time_);
  }
  if (time_ != nullptr) {
    ESP_LOGI("clockwise_hub75", "g_dt.set_rtc called with time_=%p", time_);
    g_dt.set_rtc(time_);
  }
}

static const char *const TAG = "clockwise_hub75";

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
  
  // Initialize datetime shim
  g_dt.begin();
  // Time sources are now wired from YAML via set_ha_time and set_rtc_time
  // Default to HA time source
  set_time_source(time_source_);
  
  // Create clockface based on type
  switch (clockface_type_) {
    case PACMAN:
      clockface_ = new pacman::Clockface(gfx_wrapper_);  // Pacman Clockface implements IClockface
      clockface_->setup(&g_dt);
      ESP_LOGCONFIG(TAG, "Pacman clockface initialized");
      break;
    case MARIO:
      clockface_ = new mario::Clockface(gfx_wrapper_);  // Mario Clockface implements IClockface
      clockface_->setup(&g_dt);
      ESP_LOGCONFIG(TAG, "Mario clockface initialized");
      break;
    case DUNE:
      // Use new theme-based architecture
      theme_ = new dune::DuneTheme();
      clockface_ = new StoryClockface(theme_, gfx_wrapper_);
      clockface_->setup(&g_dt);
      ESP_LOGCONFIG(TAG, "Dune clockface initialized (theme-based)");
      break;
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
  
  ESP_LOGI(TAG, "Switching clockface from %d to %d", static_cast<int>(clockface_type_), static_cast<int>(type));
  
  // Destroy the old clockface
  if (clockface_ != nullptr) {
    delete clockface_;
    clockface_ = nullptr;
  }
  
  // Destroy the old theme if present
  if (theme_ != nullptr) {
    delete theme_;
    theme_ = nullptr;
  }
  
  // Clear the display
  if (hub75_display_ != nullptr) {
    hub75_display_->fill(Color(0, 0, 0));
  }
  
  // Update the clockface type
  clockface_type_ = type;
  
  // Create the new clockface based on type
  switch (clockface_type_) {
    case PACMAN:
      clockface_ = new pacman::Clockface(gfx_wrapper_);
      clockface_->setup(&g_dt);
      ESP_LOGCONFIG(TAG, "Switched to Pacman clockface");
      break;
    case MARIO:
      clockface_ = new mario::Clockface(gfx_wrapper_);
      clockface_->setup(&g_dt);
      ESP_LOGCONFIG(TAG, "Switched to Mario clockface");
      break;
    case DUNE:
      // Use new theme-based architecture
      theme_ = new dune::DuneTheme();
      clockface_ = new StoryClockface(theme_, gfx_wrapper_);
      clockface_->setup(&g_dt);
      ESP_LOGCONFIG(TAG, "Switched to Dune clockface (theme-based)");
      break;
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