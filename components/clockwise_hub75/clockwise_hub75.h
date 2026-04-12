#pragma once


#include "esphome/core/component.h"
#include "esphome/components/hub75/hub75_component.h"
#include "esphome/components/time/real_time_clock.h"
#include "IClockface.h"
#include "CWDateTime.h"
#include <string>

extern ::CWDateTime g_dt;

// Forward declaration to avoid circular include
class GFXWrapper;

namespace esphome {
namespace clockwise_hub75 {

enum ClockfaceType {
    PACMAN = 0,
    MARIO = 1,
    CLOCK = 2
};

enum PanelColorOrder {
  RGB = 0,
  RBG = 1,
  GRB = 2,
  GBR = 3,
  BRG = 4,
  BGR = 5
};

class ClockwiseHUB75 : public PollingComponent {
 public:
  static const size_t MAX_SPECIAL_DATES = 10;
  static const size_t MAX_SPECIAL_NAME_LEN = 24;

  struct SpecialDateEntry {
    uint8_t month{0};
    uint8_t day{0};
    char name[MAX_SPECIAL_NAME_LEN + 1]{0};
    bool used{false};
  };

  void setup() override;
  void update() override { update_display_(); }
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_CONNECTION; }

  // Configuration
  void set_hub75_display(esphome::hub75::HUB75Display *display) { hub75_display_ = display; }
  void set_ha_time(time::RealTimeClock *t) { ha_time_ = t; }
  void set_ntp_time(time::RealTimeClock *t) { ntp_time_ = t; }
  void set_rtc_time(time::RealTimeClock *t) { rtc_time_ = t; }
  void set_time_source(int source);
  void set_clockface_type(ClockfaceType type) { clockface_type_ = type; }
  void set_initial_brightness(uint8_t brightness) { initial_brightness_ = brightness; }
  void set_panel_color_order(PanelColorOrder order);
  void set_special_dates(const std::string &value);

  // Control methods for Home Assistant
  void set_brightness(uint8_t brightness);
  void set_power(bool state);
  void switch_clockface(ClockfaceType type, bool force = false);
  
  // Getters for entities
  uint8_t get_brightness() const { return current_brightness_; }
  bool get_power() const { return power_state_; }
  ClockfaceType get_clockface_type() const { return clockface_type_; }
  PanelColorOrder get_panel_color_order() const { return panel_color_order_; }
  std::string get_special_dates() const { return special_dates_csv_; }
  size_t get_special_date_count() const { return special_date_count_; }
  const SpecialDateEntry *get_special_date_entries() const { return special_dates_; }
  bool is_today_special_date() const;
  std::string get_today_special_name() const;

 protected:
  esphome::hub75::HUB75Display *hub75_display_{nullptr};
  time::RealTimeClock *time_{nullptr};
  time::RealTimeClock *ha_time_{nullptr};
  time::RealTimeClock *ntp_time_{nullptr};
  time::RealTimeClock *rtc_time_{nullptr};
  int time_source_{0}; // 0 = HA, 1 = NTP, 2 = RTC
  IClockface *clockface_{nullptr};
  GFXWrapper *gfx_wrapper_{nullptr};
  
  ClockfaceType clockface_type_{PACMAN};
  PanelColorOrder panel_color_order_{RGB};
  uint8_t initial_brightness_{128};
  uint8_t current_brightness_{128};
  bool power_state_{true};
  std::string special_dates_csv_{};
  size_t special_date_count_{0};
  SpecialDateEntry special_dates_[MAX_SPECIAL_DATES];

  void set_time(time::RealTimeClock *t) { time_ = t; }
  void update_display_();
  void parse_special_dates_(const std::string &value);
  bool get_today_special_name_(std::string &name_out) const;
};

}  // namespace clockwise_hub75
}  // namespace esphome