#include <Arduino.h>

const char* PHRASES_TIME[] = {
  "TIME FLOWS",
  "THE HOUR TURNS",
  "THE MOMENT PASSES"
};

const char* PHRASES_DESERT[] = {
  "THE DESERT WATCHES",
  "SAND REMEMBERS",
  "WIND OVER DUNES"
};

const char* PHRASES_POWER[] = {
  "SPICE MUST FLOW",
  "TIME IS POWER",
  "ALL EYES OPEN"
};

const char* PHRASES_DANGER[] = {
  "THE MAKER STIRS",
  "TREAD CAREFULLY",
  "SILENCE BEFORE STORM"
};

const char* PHRASES_SURVIVAL[] = {
  "ENDURE",
  "ONLY THE PATIENT",
  "TIME SURVIVES"
};


#define COUNT_TIME      3
#define COUNT_DESERT    3
#define COUNT_POWER     3
#define COUNT_DANGER    3
#define COUNT_SURVIVAL  3

const char* selectPhrase(uint8_t act) {
  switch (act) {
    case 1: return PHRASES_DESERT[random(COUNT_DESERT)];
    case 2: return PHRASES_TIME[random(COUNT_TIME)];
    case 3: return PHRASES_POWER[random(COUNT_POWER)];
    case 4: return PHRASES_DANGER[random(COUNT_DANGER)];
    case 5: return PHRASES_SURVIVAL[random(COUNT_SURVIVAL)];
    default: return "TIME FLOWS";
  }
}
