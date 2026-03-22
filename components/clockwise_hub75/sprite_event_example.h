/*
 * Sprite-Based Event System - Example Usage
 * 
 * This file documents how to use sprites as events within the story acts.
 * 
 * ARCHITECTURE:
 * =============
 * 
 * 1. Sprite Base Class
 *    - Location: Sprite.h
 *    - Now includes virtual render(framebuffer, width, height) method
 *    - All drawable entities inherit from this
 * 
 * 2. EventTask Interface
 *    - Location: EventTask.h 
 *    - Defines execute(EventType, Sprite*, uint16_t) method
 *    - Allows sprites to respond to events
 * 
 * 3. TestShip Sprite (Example Implementation)
 *    - Location: sprite_test_ship.h/cpp
 *    - Inherits from both Sprite and EventTask
 *    - Simple example that moves a GOOMBA sprite across the display left-to-right
 *    - Can serve as template for other sprite-based events
 * 
 * 4. Story Rendering Pipeline
 *    - story_renderer.h/cpp: Now manages event sprites
 *    - Calls sprite->render() in layerEvent() during rendering
 *    - Updates/renders sprites after background but before text
 * 
 * 5. Story Clockface Integration
 *    - story_Clockface.h/cpp: Creates and manages TestShip instance
 *    - Passes sprite to renderer for rendering
 *    - Updates sprite position each frame
 * 
 * USAGE EXAMPLE - Creating Your Own Sprite-Based Event:
 * ====================================================
 * 
 * 1. Create a new class inheriting from Sprite and EventTask:
 * 
 *    class MySpaceShip : public Sprite, public EventTask {
 *    private:
 *      const unsigned short* _spriteData;
 *      uint8_t _speed;
 *      uint32_t _lastUpdate;
 *    
 *    public:
 *      MySpaceShip();
 *      void update();
 *      void render(uint16_t* framebuffer, uint8_t width, uint8_t height) const override;
 *      void execute(EventType event, Sprite* caller, uint16_t value = 0) override;
 *      const char* name() override;
 *    };
 * 
 * 2. Implement the class to handle movement and rendering:
 * 
 *    void MySpaceShip::update() {
 *      // Update position each frame
 *      _x += _speed;
 *      if (_x > DISPLAY_WIDTH) _x = -_width;
 *    }
 *    
 *    void MySpaceShip::render(uint16_t* framebuffer, uint8_t fbWidth, uint8_t fbHeight) const {
 *      // Draw sprite at current position to framebuffer
 *      // Must handle transparency and bounds checking
 *      for (int y = 0; y < _height; y++) {
 *        for (int x = 0; x < _width; x++) {
 *          int px = _x + x;
 *          int py = _y + y;
 *          if (px >= 0 && px < fbWidth && py >= 0 && py < fbHeight) {
 *            // Get pixel from sprite data, skip transparency
 *            uint16_t pixel = _spriteData[y * _width + x];
 *            if (pixel != TRANSPARENT) {
 *              framebuffer[py * fbWidth + px] = pixel;
 *            }
 *          }
 *        }
 *      }
 *    }
 * 
 * 3. Integrate into StoryClockface (or other clockface):
 * 
 *    In story_Clockface.h:
 *    - Add member variable: MySpaceShip _spaceShip;
 *    
 *    In story_Clockface::setup():
 *    - Initialize: _spaceShip.setActive(true);
 *    - Pass to renderer: _renderer.setEventSprite(&_spaceShip);
 *    
 *    In story_Clockface::update():
 *    - Update position: _spaceShip.update();
 *    - Renderer will automatically call render() via layerEvent()
 * 
 * RENDERING LAYERS (in order):
 * ============================
 * 
 * L0: Clear framebuffer
 * L1: Background (act-specific image)
 * L2: Ambient effects (theme-specific)
 * L3: EVENT SPRITES (your sprites here!)
 * L4: Time display (HH:MM format)
 * L5: Text overlay (phrases)
 * 
 * This means sprites render on top of background but behind text, allowing
 * for natural layering of visual elements.
 * 
 * EVENT TYPES:
 * ============
 * 
 * From EventTask.h:
 * - MOVE: Sprite should move
 * - COLLISION: Sprite collided with something
 * - COLLISION_JUMP: Special collision (no time update)
 * - SKY_COLOR_CHANGED: Background color changed
 * 
 * Implement execute() to respond to these events:
 * 
 *    void MySpaceShip::execute(EventType event, Sprite* caller, uint16_t value) {
 *      switch (event) {
 *        case MOVE:
 *          move(LEFT);
 *          break;
 *        case COLLISION:
 *          // Handle collision with other sprite
 *          break;
 *        default:
 *          break;
 *      }
 *    }
 * 
 * EXAMPLE: SANDWORM EVENT
 * =======================
 * 
 * To create a sandworm that occasionally emerges from sand:
 * 
 * 1. Create sandworm sprite data (or reuse existing asset)
 * 2. Implement SandwormEvent class:
 *    - Position: starts below screen
 *    - Movement: rises up, stays briefly, descends
 *    - Timing: triggered at specific act cycles
 *    - Visual: use semi-transparent or color-shift for "emerging"
 * 
 * 3. Control via act duration:
 *    - Each act lasts ~4 hours
 *    - Schedule sandworm to appear at 1-hour intervals
 *    - Use _now - _storyStartMs to calculate timing
 * 
 * TESTING:
 * ========
 * 
 * Current setup automatically creates and displays TestShip:
 * - Sprite: 8x8 GOOMBA (from mario_assets.h)
 * - Movement: Horizontal sweep left to right
 * - Speed: 2 pixels per 50ms update
 * - Visibility: Display width (64 pixels)
 * 
 * To disable TestShip during development:
 * - In story_Clockface::setup(): _testShip.setActive(false);
 * 
 * To implement custom logic:
 * - Create new sprite class (see examples above)
 * - Add to StoryClockface similar to TestShip
 * - Implement update() and render() methods
 * - Pass to renderer via setEventSprite()
 */
