#include "CWDateTime.h"
#include "esphome/core/application.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/core/log.h"

using namespace esphome;
using namespace esphome::time;

static const char *const TAG = "CWDateTime";

void CWDateTime::set_rtc(esphome::time::RealTimeClock *rtc) {
  rtc_ = rtc;
  if (rtc_)
    ESP_LOGI(TAG, "RTC successfully linked!");
  else
    ESP_LOGW(TAG, "RTC pointer cleared or invalid!");
}


void CWDateTime::begin() {
  ESP_LOGI(TAG, "CWDateTime initialized. Waiting for RTC to be assigned via YAML.");
}

String CWDateTime::getFormattedTime() {
  if (!rtc_) return "00:00:00";

  esphome::ESPTime t = rtc_->now();
  if (!t.is_valid()) return "00:00:00";

  char buf[16];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", t.hour, t.minute, t.second);
  return String(buf);
}

String CWDateTime::getFormattedTime(const char* /*format*/) {
  // For now, ignore the format string
  return getFormattedTime();
}

int CWDateTime::getHour() {
  if (!rtc_) return 0;
  esphome::ESPTime t = rtc_->now();
  return t.is_valid() ? t.hour : 0;
}

int CWDateTime::getMinute() {
  if (!rtc_) return 0;
  esphome::ESPTime t = rtc_->now();
  return t.is_valid() ? t.minute : 0;
}

int CWDateTime::getSecond() {
  if (!rtc_) return 0;
  esphome::ESPTime t = rtc_->now();
  return t.is_valid() ? t.second : 0;
}

int CWDateTime::getDay() {
  if (!rtc_) return 1;
  esphome::ESPTime t = rtc_->now();
  return t.is_valid() ? t.day_of_month : 1;
}

int CWDateTime::getMonth() {
  if (!rtc_) return 1;
  esphome::ESPTime t = rtc_->now();
  return t.is_valid() ? t.month : 1;
}

int CWDateTime::getWeekday() {
  if (!rtc_) return 1;
  esphome::ESPTime t = rtc_->now();
  if (!t.is_valid()) return 1;

  // Compute weekday (1=Monday..7=Sunday)
  int q = t.day_of_month;
  int m = t.month < 3 ? t.month + 12 : t.month;
  int K = (t.year % 100) - (t.month < 3 ? 1 : 0);
  int J = (t.year / 100);
  int h = (q + 13*(m+1)/5 + K + K/4 + J/4 + 5*J) % 7;
  int d = ((h + 5) % 7) + 1;
  return d;
}

long CWDateTime::getMilliseconds() 
{
  return 0;
}

char *CWDateTime::getHour(const char *format) {
  static char buffer[3] = {'\0'};
  if (!rtc_) { strncpy(buffer, "00", sizeof(buffer)); return buffer; }

  esphome::ESPTime t = rtc_->now();
  if (!t.is_valid()) { strncpy(buffer, "00", sizeof(buffer)); return buffer; }

  int hour = t.hour;
  if (!use24hFormat_) {
    // convert 0..23 -> 12-hour 1..12
    hour = hour % 12;
    if (hour == 0) hour = 12;
  }
  snprintf(buffer, sizeof(buffer), "%02d", hour);
  return buffer;
}

char *CWDateTime::getMinute(const char *format) {
  static char buffer[3] = {'\0'};
  if (!rtc_) { strncpy(buffer, "00", sizeof(buffer)); return buffer; }

  esphome::ESPTime t = rtc_->now();
  if (!t.is_valid()) { strncpy(buffer, "00", sizeof(buffer)); return buffer; }

  int minute = t.minute;
  bool nozero = (format && strstr(format, "nozero") != nullptr);
  if (nozero) {
    snprintf(buffer, sizeof(buffer), "%d", minute);
  } else {
    snprintf(buffer, sizeof(buffer), "%02d", minute);
  }
  return buffer;
}

bool CWDateTime::isAM() {
  return getHour() < 12;
}

bool CWDateTime::is24hFormat() {
  return use24hFormat_;
}

















