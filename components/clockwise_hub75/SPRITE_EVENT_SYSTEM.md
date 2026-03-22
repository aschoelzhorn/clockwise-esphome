# Sprite-Based Event System Implementation

## Overview

You now have a complete sprite-based event system integrated into your story clockface. This allows sprites (like moving spaceships, sandworms, etc.) to be rendered as part of the act events.

## What Was Created

### 1. **sprite_test_ship.h / sprite_test_ship.cpp**
   - New sprite class that inherits from both `Sprite` and `EventTask`
   - Uses the GOOMBA sprite from mario assets as a test visual
   - Moves horizontally across the 64x64 display from left to right
   - Wraps around when reaching the edge
   - **Location**: `/components/clockwise_hub75/sprite_test_ship.*`

### 2. **Updated Sprite.h**
   - Added virtual `render()` method to base Sprite class
   - Signature: `virtual void render(uint16_t* framebuffer, uint8_t fbWidth, uint8_t fbHeight) {}`
   - Allows all sprites to implement their own rendering logic

### 3. **Updated story_renderer.h / story_renderer.cpp**
   - Added `Sprite* event_sprite_` member to manage event sprites
   - Added public methods:
     - `setEventSprite(Sprite* sprite)` - Set which sprite to render
     - `getEventSprite()` - Get current event sprite
   - Updated `layerEvent()` to call `sprite->render(framebuffer, 64, 64)`
   - Event sprites render in Layer 3 (between background and text)

### 4. **Updated story_Clockface.h / story_Clockface.cpp**
   - Added `#include "sprite_test_ship.h"`
   - Added `TestShip _testShip` member variable
   - In `setup()`: Initializes and registers TestShip with renderer
   - In `update()`: Calls `_testShip.update()` each frame
   - Sprite automatically renders via the renderer's layerEvent()

### 5. **Documentation: sprite_event_example.h**
   - Comprehensive guide on the sprite event system architecture
   - Examples for creating custom sprites (spaceship, sandworm, etc.)
   - Event type documentation
   - Usage patterns and integration guide

## How It Works

### Rendering Pipeline

The story clockface renders in layers (bottom to top):

```
L0: Clear framebuffer (black)
L1: Background image (act-specific imagery)
L2: Ambient effects (placeholder)
L3: EVENT SPRITES ← Your sprites render here!
L4: Time display (HH:MM)
L5: Text overlay (story phrases)
```

### Frame-by-Frame Execution

1. **Update Phase** (each frame):
   - `StoryClockface::update()` calls `_testShip.update()`
   - TestShip increments its X position
   - Wraps around when reaching display edge

2. **Render Phase** (each frame):
   - Renderer builds framebuffer in layers
   - `layerEvent()` calls `sprite->render(framebuffer, 64, 64)`
   - Sprite draws itself pixel-by-pixel to framebuffer
   - Handles transparency (skips TRANSPARENT color)
   - Bounds checking prevents off-screen drawing

3. **Display Phase**:
   - Framebuffer sent to display via `drawRGBBitmap()`

## Current Test Setup

The TestShip sprite is now active by default:

- **Visual**: 8x8 GOOMBA sprite from mario assets
- **Movement**: Travels left to right across display at 2px per 50ms
- **Loop time**: ~1.6 seconds to traverse the full 64px display
- **Position**: Y=20 (middle-ish of display)

## Creating Custom Sprites

See `sprite_event_example.h` for complete examples, but here's the quick version:

### 1. Create Header File

```cpp
#pragma once
#include "Sprite.h"
#include "EventTask.h"

class MySprite : public Sprite, public EventTask {
public:
  MySprite();
  void update();
  void render(uint16_t* framebuffer, uint8_t fbWidth, uint8_t fbHeight) const override;
  void execute(EventType event, Sprite* caller, uint16_t value = 0) override;
  const char* name() override;
};
```

### 2. Implement Movement & Rendering

```cpp
void MySprite::update() {
  _x += speed_;  // Move each frame
  if (_x > 64) _x = -_width;  // Wrap around
}

void MySprite::render(uint16_t* fb, uint8_t w, uint8_t h) const {
  for (int y = 0; y < height_; y++) {
    for (int x = 0; x < width_; x++) {
      int px = _x + x;
      int py = _y + y;
      if (px >= 0 && px < w && py >= 0 && py < h) {
        uint16_t pixel = sprite_data_[y * width_ + x];
        if (pixel != TRANSPARENT) {
          fb[py * w + px] = pixel;
        }
      }
    }
  }
}
```

### 3. Integrate into Story Clockface

In `story_Clockface.h`:
```cpp
#include "my_sprite.h"
// ...
MySprite _mySprite;  // Add member
```

In `story_Clockface::setup()`:
```cpp
_mySprite.setActive(true);
_renderer.setEventSprite(&_mySprite);
```

In `story_Clockface::update()`:
```cpp
_mySprite.update();  // Add this line
```

The renderer will automatically render it!

## Event System

Sprites can respond to events via the `execute()` method:

```cpp
void MySprite::execute(EventType event, Sprite* caller, uint16_t value) {
  switch (event) {
    case MOVE:
      // Handle MOVE event
      break;
    case COLLISION:
      // Handle collision with another sprite
      break;
    default:
      break;
  }
}
```

Available event types (from EventTask.h):
- `MOVE` - Movement signal
- `COLLISION` - Collision with object
- `COLLISION_JUMP` - Special collision without time update
- `SKY_COLOR_CHANGED` - Background color changed

## Example Implementations

### Sandworm Event

```cpp
class SandwormEvent : public Sprite, public EventTask {
private:
  uint32_t _lastEmergeTime = 0;
  const uint32_t EMERGE_INTERVAL = 30000;  // 30 seconds
  enum State { HIDDEN, RISING, EMERGING, DESCENDING };
  State _state = HIDDEN;
  
public:
  void update() override {
    uint32_t now = millis();
    if (now - _lastEmergeTime > EMERGE_INTERVAL) {
      _lastEmergeTime = now;
      _state = RISING;
      _y = 64;  // Start below screen
    }
    
    // Animate based on state...
  }
  
  void render(...) override {
    // Draw sandworm sprite with animation
  }
};
```

### Moving Spaceship

```cpp
class SpaceshipEvent : public Sprite, public EventTask {
private:
  const unsigned short* _spriteData;
  uint8_t _speed = 2;
  
public:
  void update() override {
    _x += _speed;
    if (_x > 64) _x = -16;  // Wrap across screen
  }
  
  void render(...) override {
    // Draw spaceship with current position
  }
};
```

## Files Modified

1. ✅ `Sprite.h` - Added virtual render method
2. ✅ `story_renderer.h` - Added sprite member and methods
3. ✅ `story_renderer.cpp` - Implemented layerEvent render call
4. ✅ `story_Clockface.h` - Added TestShip member and include
5. ✅ `story_Clockface.cpp` - Initialize and update TestShip

## Files Created

1. ✅ `sprite_test_ship.h` - TestShip header
2. ✅ `sprite_test_ship.cpp` - TestShip implementation
3. ✅ `sprite_event_example.h` - Documentation with examples

## Testing

The TestShip sprite is automatically active when the story clockface runs. You should see a small GOOMBA sprite moving across the display from left to right, continuously.

To verify it's working:
- Look for the GOOMBA sprite (8x8 brown character) moving horizontally
- Should complete a full traverse in ~1.6 seconds at Y=20
- Should loop continuously

## Next Steps

1. **Test the current implementation** with your display
2. **Create custom sprites** for your specific events (sandworm, spaceship, etc.)
3. **Add timing logic** to control when sprites appear
4. **Implement event responses** for interactive elements
5. **Integrate with act cycles** to show different sprites in different acts

## Troubleshooting

**Sprite not visible?**
- Check that `_testShip.setActive(true)` is called in setup()
- Verify renderer is calling `layerEvent()` 
- Check sprite coordinates (_x, _y) are within display bounds (0-63)

**Sprite rendering wrong?**
- Check sprite data format (should be uint16_t array of RGB565 colors)
- Verify TRANSPARENT color is correct (0xFEFE for mario assets)
- Check _width and _height match sprite data dimensions

**Performance issues?**
- Reduce update frequency (increase _updateIntervalMs in TestShip)
- Use smaller sprites or cache sprite rendering
- Profile framebuffer operations
