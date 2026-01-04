#pragma once

#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"
#include "../clockwise_hub75.h"

namespace esphome {
namespace clockwise_hub75 {

class ClockwiseSwitch : public switch_::Switch, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_clockwise_hub75(ClockwiseHUB75 *parent) { parent_ = parent; }

 protected:
  void write_state(bool state) override;
  ClockwiseHUB75 *parent_{nullptr};
};

}  // namespace clockwise_hub75
}  // namespace esphome