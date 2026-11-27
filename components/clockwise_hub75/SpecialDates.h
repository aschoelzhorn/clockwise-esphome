#pragma once

// Lightweight special-dates query API.
// ClockwiseHUB75 registers itself via special_dates_register() during setup().
// Clockfaces query via special_date_is_today() / special_date_today_name()
// without depending on ClockwiseHUB75 directly.

typedef bool        (*SpecialDateCheckFn)(void *ctx);
typedef const char *(*SpecialDateNameFn) (void *ctx);

void        special_dates_register(void *ctx, SpecialDateCheckFn is_today, SpecialDateNameFn name);
bool        special_date_is_today();
const char *special_date_today_name();  // Returns "" when not a special date
