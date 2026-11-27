#pragma once

#include <stdint.h>

class NightModeSchedule {
 public:
  constexpr void set_start(uint32_t seconds) { start_ = seconds; }
  constexpr void set_end(uint32_t seconds) { end_ = seconds; }

  constexpr bool is_active(uint32_t seconds) const {
    if (start_ == end_) return false;
    if (start_ < end_) return seconds >= start_ && seconds < end_;
    return seconds >= start_ || seconds < end_;
  }

 private:
  uint32_t start_{20 * 3600};
  uint32_t end_{6 * 3600};
};
