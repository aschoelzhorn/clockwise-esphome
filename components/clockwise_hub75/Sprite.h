#pragma once

#include <Arduino.h>

class Sprite {

  protected:
    int8_t _x = 0;
    int8_t _y = 0;
    uint8_t _width = 0;
    uint8_t _height = 0;

  public:
    boolean collidedWith(Sprite* sprite);
    void logPosition();

    virtual const char* name();
    
    // Accessors for rendering (renderer is responsible for drawing, not sprite)
    int8_t getX() const { return _x; }
    int8_t getY() const { return _y; }
    uint8_t getWidth() const { return _width; }
    uint8_t getHeight() const { return _height; }
    
    // Virtual accessor for sprite data (pure sprite data only, no rendering logic)
    virtual const unsigned short* getSpriteData() const { return nullptr; }
};
