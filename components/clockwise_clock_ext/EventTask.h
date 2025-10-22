#pragma once

#include <Arduino.h>
#include "Sprite.h"

enum EventType {
    MOVE, 
    COLLISION,
    COLLISION_JUMP,  // New event type for collision-based jumps (no time update)
    TIME_UPDATE,     // New event type for time-based updates
    MOVE_NO_HIT,     // New event type for movement that shouldn't hit blocks
    REDRAW_BLOCK     // tells blocks to redraw
};

class EventTask {
  public:
    virtual void execute(EventType event, Sprite* caller) = 0;
};



