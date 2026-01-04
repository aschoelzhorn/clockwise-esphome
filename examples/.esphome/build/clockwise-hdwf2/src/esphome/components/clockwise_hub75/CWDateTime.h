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
  
  String getFormattedTime();
  String getFormattedTime(const char* format);

  char *getHour(const char *format);
  char *getMinute(const char *format);
  int getHour();
  int getMinute();
  int getSecond();
  long getMilliseconds();

  int getDay();
  int getMonth();
  int getWeekday();

  bool isAM();
  bool is24hFormat();
};
