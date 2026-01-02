#pragma once

#include <Arduino.h>

const unsigned short DIM_SAND = 0xC618; // ACT I: Desert Sleeps (dim sand)
const unsigned short SPICE_AMBER = 0xFCA0; // ACT II: Spice Awakens (spice amber)
const unsigned short HIGH_CONTRAST_WHITE = 0xFFFF; // ACT III: Watchers (high contrast, white)
const unsigned short BRIGHT_SAND = 0xFFE0; // ACT IV: Maker Stirs (bright sand)
const unsigned short RED_DANGER = 0xF800; // ACT V: Storm of Fate (red, danger)
const unsigned short COOL_BROWN = 0x8410; // ACT VI: Silence & Survival (cool brown)


extern const uint8_t dune_font5x7[][5];
uint8_t duneFontIndex(char c);

