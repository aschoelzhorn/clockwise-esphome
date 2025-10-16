#include "clockwise_clock.h"
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#include "CWDateTime.h"
#include "Locator.h"

#include "MarioClockface.h"
#include "PacmanClockface.h"

namespace clockwise_clock_ext {

static ::CWDateTime g_dt;   // implemented by shim (CWDateTime.cpp)

void ClockwiseClock::setup() {
  HUB75_I2S_CFG cfg(w_, h_, 1);
  cfg.gpio.r1=r1_; cfg.gpio.g1=g1_; cfg.gpio.b1=b1_;
  cfg.gpio.r2=r2_; cfg.gpio.g2=g2_; cfg.gpio.b2=b2_;
  cfg.gpio.a=a_; cfg.gpio.b=bb_; cfg.gpio.c=c_; cfg.gpio.d=d_; cfg.gpio.e=e_;
  cfg.gpio.lat=lat_; cfg.gpio.oe=oe_; cfg.gpio.clk=clk_;
  cfg.clkphase = clkphase_;
  if (driver_=="SHIFTREG") {cfg.driver = HUB75_I2S_CFG::SHIFTREG;}
  else if (driver_=="FM6124") {cfg.driver = HUB75_I2S_CFG::FM6124;}
  else if (driver_=="FM6126A") {cfg.driver = HUB75_I2S_CFG::FM6126A;}
  else if (driver_=="ICN2038S") {cfg.driver = HUB75_I2S_CFG::ICN2038S;}
  else if (driver_=="MBI5124") {cfg.driver = HUB75_I2S_CFG::MBI5124;}
  else if (driver_=="DP3246") {cfg.driver = HUB75_I2S_CFG::DP3246;}
  else { cfg.driver = HUB75_I2S_CFG::SHIFTREG;; ESP_LOGW("clockwise", "Invalid driver '%s', defaulting to SHIFTREG", driver_.c_str()); }

  if (i2c_speed_ == "8M") {  cfg.i2sspeed = HUB75_I2S_CFG::HZ_8M; }
  else if (i2c_speed_ == "10M") { cfg.i2sspeed = HUB75_I2S_CFG::HZ_10M; }
  else if (i2c_speed_ == "15M") { cfg.i2sspeed = HUB75_I2S_CFG::HZ_15M; }
  else if (i2c_speed_ == "16M") { cfg.i2sspeed = HUB75_I2S_CFG::HZ_16M; }
  else if (i2c_speed_ == "20M") { cfg.i2sspeed = HUB75_I2S_CFG::HZ_20M; }
  else { cfg.i2sspeed = HUB75_I2S_CFG::HZ_8M; ESP_LOGW("clockwise", "Invalid i2c_speed '%s', defaulting to 8M", i2c_speed_.c_str()); }

  panel_ = new MatrixPanel_I2S_DMA(cfg);
  panel_->begin();
  panel_->setBrightness8(brightness_);
  panel_->clearScreen();

  // Provide Adafruit_GFX* to the Clockwise gfx engine (via Locator)
  Locator::provide(panel_);

  g_dt.begin(time_);
 
  // Select clockface implementation
  if (clockface_type_ == 1) { // TODO make enum or const, 0: pacman, 1: mario
    clockface_ = new MarioClockface(static_cast<Adafruit_GFX*>(panel_));
  } else {
    clockface_ = new PacmanClockface(static_cast<Adafruit_GFX*>(panel_));
  }
  clockface_->setup(&g_dt);
}

void ClockwiseClock::update() {
  if (clockface_) clockface_->update();
}

  void ClockwiseClock::set_clockface_type(const std::string &type) {
    int new_type = 0;
    if (type == "pacman") {
      new_type = 0;
    } else if (type == "mario") {
      new_type = 1;
    }
    set_clockface_type(new_type);  // call the int version
  }

void ClockwiseClock::set_clockface_type(int type) {
  if (type == clockface_type_) return;
  clockface_type_ = type;
  if (clockface_) {
    delete clockface_;
    clockface_ = nullptr;
  }
  if (clockface_type_ == 0) { // 0: pacman
    clockface_ = new PacmanClockface(static_cast<Adafruit_GFX*>(panel_));
  } else { // 1: mario
    clockface_ = new MarioClockface(static_cast<Adafruit_GFX*>(panel_));
  }
  if (clockface_) clockface_->setup(&g_dt);
}
}
