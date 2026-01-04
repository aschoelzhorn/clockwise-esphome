#pragma once
#include <stdint.h>
#include <stddef.h>

namespace dune {

class Act {

private:
    uint8_t _id;
    const char* _name;
    const char** _phrases;
    size_t _phraseCount = COUNT_PHRASES;
    const uint16_t* _background;
    int _lastPhraseIndex = -1;
    uint32_t _lastPhraseUpdate = 0;
    uint16_t _fontColor;

public:
    Act();
    Act(uint8_t id, const char* name, const char** phrases, uint16_t fontColor, const uint16_t* background);
    uint8_t getId();
    const char* getName();
    const char* getPhrase(size_t index);
    const char* getPhrase();
    size_t getPhraseCount();
    const uint16_t* getBackground();
    uint16_t getFontColor();

};

}
