#include "clockwise_hub75.h"
#include "esphome/core/log.h"
#include "Locator.h"
#include "GFXWrapper.h"
#include "pacman_Clockface.h"  // Pacman clockface implementation
#include "mario_Clockface.h"  // For Mario face

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
  
  // Setup preferences
  pref_clockface_ = global_preferences->make_preference<uint8_t>(fnv1_hash("clockface"));
  pref_brightness_ = global_preferences->make_preference<uint8_t>(fnv1_hash("brightness"));
  
  // Load saved clockface type
  uint8_t saved_type = 0;
  if (pref_clockface_.load(&saved_type)) {
    clockface_type_ = static_cast<ClockfaceType>(saved_type);
    ESP_LOGI(TAG, "Restored clockface type: %d", saved_type);
  }
  
  // Load saved brightness
  uint8_t saved_brightness = 0;
  if (pref_brightness_.load(&saved_brightness)) {
    current_brightness_ = saved_brightness;
    ESP_LOGI(TAG, "Restored brightness: %d", saved_brightness);
  } else {
    current_brightness_ = initial_brightness_;
  }
  
  // Apply brightness
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
  
  // Create clockface based on (potentially restored) type
  switch (clockface_type_) {
    case PACMAN:
      clockface_ = new pacman::Clockface(gfx_wrapper_);
      clockface_->setup(&g_dt);
      ESP_LOGCONFIG(TAG, "Pacman clockface initialized");
      break;
    case MARIO:
      clockface_ = new mario::Clockface(gfx_wrapper_);
      clockface_->setup(&g_dt);
      ESP_LOGCONFIG(TAG, "Mario clockface initialized");
      break;
    case CLOCK:
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
  pref_brightness_.save(&brightness);
  ESP_LOGD(TAG, "Brightness set to %d (saved)", brightness);
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
    ESP_LOGD(TAG, "Already using clockface type %d", static_cast<int>(type));
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
  
  // Update and save the clockface type
  clockface_type_ = type;
  uint8_t type_value = static_cast<uint8_t>(type);
  pref_clockface_.save(&type_value);
  
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
    case CLOCK:
      ESP_LOGW(TAG, "Basic clock clockface not yet implemented, staying black");
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