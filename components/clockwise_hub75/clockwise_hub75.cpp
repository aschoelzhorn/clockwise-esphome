#include "clockwise_hub75.h"
#include "esphome/core/log.h"
#include "Locator.h"
#include "GFXWrapper.h"
#include "Clockface.h"  // For Pacman face (default)

namespace esphome {
namespace clockwise_hub75 {

static const char *const TAG = "clockwise_hub75";
static ::CWDateTime g_dt;  // Global datetime instance (shim)

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
  if (time_ != nullptr) {
    g_dt.set_rtc(time_);
  }
  
  // Create clockface based on type
  switch (clockface_type_) {
    case PACMAN:
      clockface_ = new Clockface(gfx_wrapper_);  // Clockface implements IClockface
      clockface_->setup(&g_dt);
      ESP_LOGCONFIG(TAG, "Pacman clockface initialized");
      break;
    case MARIO:
      // TODO: Replace with MarioClockface (must implement IClockface)
      ESP_LOGW(TAG, "Mario clockface not yet implemented");
      break;
    case CLOCK:
      // TODO: Replace with BasicClockface (must implement IClockface)
      ESP_LOGW(TAG, "Basic clock clockface not yet implemented");
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
  clockface_type_ = type;
  ESP_LOGD(TAG, "Clockface switched to %d", static_cast<int>(type));
  // TODO: Implement dynamic clockface switching
  // This would require destroying old clockface and creating new one
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