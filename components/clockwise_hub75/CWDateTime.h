#pragma once

#include "esphome/components/time/real_time_clock.h"
#include <Arduino.h>

class CWDateTime {
private:
  esphome::time::RealTimeClock *rtc_ = nullptr;
  bool use24hFormat_ = true;

public:
  void begin();
  void set_rtc(esphome::time::RealTimeClock *rtc);
  void set_use_24h_format(bool use24hFormat);
  
  String getFormattedTime();
  String getFormattedTime(const char* format);

  char *getHour(const char *format);
  char *getMinute(const char *format);
  int getHour();
  int getMinute();
  int getSecond();
  uint32_t getSecondsSinceMidnight();
  long getMilliseconds();

  int getDay();
  int getMonth();
  int getWeekday();

  bool isAM();
  bool is24hFormat();
};
