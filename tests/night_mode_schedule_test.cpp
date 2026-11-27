#include "../components/clockwise_hub75/NightModeSchedule.h"

constexpr bool test_default_overnight() {
  NightModeSchedule schedule;
  return !schedule.is_active(20 * 3600 - 1) && schedule.is_active(20 * 3600) &&
         schedule.is_active(0) && schedule.is_active(6 * 3600 - 1) && !schedule.is_active(6 * 3600);
}

constexpr bool test_same_day() {
  NightModeSchedule schedule;
  schedule.set_start(9 * 3600 + 30);
  schedule.set_end(17 * 3600 + 45);
  return !schedule.is_active(9 * 3600 + 29) && schedule.is_active(9 * 3600 + 30) &&
         schedule.is_active(17 * 3600 + 44) && !schedule.is_active(17 * 3600 + 45) &&
         !schedule.is_active(0);
}

constexpr bool test_equal_times_disable() {
  NightModeSchedule schedule;
  schedule.set_start(0);
  schedule.set_end(0);
  return !schedule.is_active(0) && !schedule.is_active(86399);
}

static_assert(test_default_overnight(), "overnight boundaries");
static_assert(test_same_day(), "same-day boundaries and seconds");
static_assert(test_equal_times_disable(), "equal times disable night mode");
