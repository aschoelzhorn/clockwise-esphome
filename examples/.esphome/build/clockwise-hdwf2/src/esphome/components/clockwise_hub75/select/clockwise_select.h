#pragma once

#include "esphome/core/component.h"
#include "esphome/components/select/select.h"
#include "../clockwise_hub75.h"

namespace esphome {
namespace clockwise_hub75 {

class ClockwiseSelect : public select::Select, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_clockwise_hub75(ClockwiseHUB75 *parent) { parent_ = parent; }

 protected:
  void control(const std::string &value) override;
  ClockwiseHUB75 *parent_{nullptr};
  
  std::string clockface_type_to_string(ClockfaceType type);
  ClockfaceType string_to_clockface_type(const std::string &str);
};

}  // namespace clockwise_hub75
}  // namespace esphome