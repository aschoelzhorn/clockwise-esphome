#pragma once

#include "esphome/core/component.h"
#include "esphome/components/time/real_time_clock.h"

#include "IClockface.h"
#include "Tile.h"
#include "Sprite.h"
#include "ColorUtil.h"
#include "EventBus.h"
#include "EventTask.h"
#include "ImageUtils.h"
#include "Locator.h"
#include "Macros.h"
#include "Object.h"
#include "Game.h"

class MatrixPanel_I2S_DMA;  // DMA driver
struct HUB75_I2S_CFG;

namespace clockwise_clock_ext {

class ClockwiseClock : public esphome::PollingComponent {
 public:
  ClockwiseClock() : PollingComponent(16) {}
  void set_dimensions(int w,int h){w_=w;h_=h;}
  void set_brightness(uint8_t b){brightness_=b;}
  void set_driver(const std::string& d){driver_=d;}
  void set_clkphase(bool v){clkphase_=v;}
  void set_time(esphome::time::RealTimeClock* t){time_=t;}
  void set_i2c_speed(const std::string& s){i2c_speed_=s;}
  void set_clockface_type(int type);
  void set_clockface_type(const std::string &type);
  int get_clockface_type() { return clockface_type_; }

  // void set_clockface_select(esphome::select::Select *select) {
  //   this->clockface_select_ = select;
  // }

  // pins
  void set_r1_pin(int v){r1_=v;}   void set_g1_pin(int v){g1_=v;} void set_b1_pin(int v){b1_=v;}
  void set_r2_pin(int v){r2_=v;}   void set_g2_pin(int v){g2_=v;} void set_b2_pin(int v){b2_=v;}
  void set_a_pin(int v){a_=v;}     void set_b_pin(int v){bb_=v;}  void set_c_pin(int v){c_=v;}
  void set_d_pin(int v){d_=v;}     void set_e_pin(int v){e_=v;}
  void set_lat_pin(int v){lat_=v;} void set_oe_pin(int v){oe_=v;} void set_clk_pin(int v){clk_=v;}

  void setup() override;
  void update() override;

protected:
  uint8_t w_{64}, h_{64}; uint8_t brightness_{180}; bool clkphase_{true};
  std::string driver_{"FM6126A"};
  std::string i2c_speed_{"8M"};
  uint8_t r1_,g1_,b1_, r2_,g2_,b2_, a_,bb_,c_,d_,e_, lat_,oe_,clk_;

  ::MatrixPanel_I2S_DMA* panel_{nullptr};
  IClockface* clockface_{nullptr};  // the real clock face (polymorphic)
  int clockface_type_{0}; // default, can be set from YAML or runtime, // 0: pacman, 1: mario

  esphome::time::RealTimeClock* time_{nullptr};
  //esphome::select::Select *clockface_select_;
};

}
