#include "IStoryTheme.h"
#include "CWDateTime.h"

uint8_t IStoryTheme::getCurrentActId(const CWDateTime& dt) const {
  uint8_t actCount = getActCount();
  uint8_t hour = dt.getHour();
  
  // Divide 24 hours evenly among acts
  // E.g., 6 acts = 4 hours per act
  uint32_t secondsPerAct = 86400 / actCount;
  uint32_t secondsInDay = hour * 3600 + dt.getMinute() * 60 + dt.getSecond();
  
  return (secondsInDay / secondsPerAct) % actCount;
}
