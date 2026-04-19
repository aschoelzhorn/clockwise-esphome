#include "SpecialDates.h"

static void              *s_ctx      = nullptr;
static SpecialDateCheckFn s_is_today = nullptr;
static SpecialDateNameFn  s_name     = nullptr;

void special_dates_register(void *ctx, SpecialDateCheckFn is_today, SpecialDateNameFn name) {
  s_ctx      = ctx;
  s_is_today = is_today;
  s_name     = name;
}

bool special_date_is_today() {
  return s_is_today != nullptr && s_is_today(s_ctx);
}

const char *special_date_today_name() {
  return (s_name != nullptr) ? s_name(s_ctx) : "";
}
