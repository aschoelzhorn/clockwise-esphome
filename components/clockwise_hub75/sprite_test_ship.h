#pragma once

#include <Arduino.h>
#include "Sprite.h"
#include "EventTask.h"
#include "mario_assets.h"
#include "Game.h"

namespace story {

/**
 * TestShip - A simple sprite that moves across the display for testing event-based rendering
 * 
 * This sprite:
 * - Inherits from both Sprite and EventTask
 * - Uses the GOOMBA sprite from mario assets for visuals
 * - Moves horizontally across the display
 * - Can be rendered and updated within the story rendering pipeline
 */
class TestShip : public Sprite, public EventTask {
private:
  const unsigned short* _sprite;
  Direction _direction = RIGHT;
  uint8_t _pace = 2;  // Pixels per update
  uint32_t _lastUpdate = 0;
  uint32_t _updateIntervalMs = 50;  // Update every 50ms
  bool _active = true;

  void drawSprite(uint16_t* framebuffer, uint8_t fbWidth, uint8_t fbHeight) const;

public:
  TestShip();
  ~TestShip();

  // Sprite interface
  const char* name() override;

  // EventTask interface
  void execute(EventType event, Sprite* caller, uint16_t value = 0) override;

  // Update and state management
  void update();
  bool isActive() const { return _active; }
  void setActive(bool active) { _active = active; }
  
  // Provide sprite data for renderer (no rendering logic in sprite itself)
  const unsigned short* getSpriteData() const override { return _sprite; }
};

}  // namespace story
