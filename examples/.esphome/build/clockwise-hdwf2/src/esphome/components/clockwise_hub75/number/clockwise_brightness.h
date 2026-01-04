#pragma once

#include "esphome/core/component.h"
#include "esphome/components/number/number.h"
#include "../clockwise_hub75.h"

namespace esphome {
namespace clockwise_hub75 {

class ClockwiseBrightness : public number::Number, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_clockwise_hub75(ClockwiseHUB75 *parent) { parent_ = parent; }

 protected:
  void control(float value) override;
  ClockwiseHUB75 *parent_{nullptr};
};

}  // namespace clockwise_hub75
}  // namespace esphome