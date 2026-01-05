#pragma once
#include <stdint.h>
#include <stddef.h>
#include "dune_phrases.h"

namespace dune {

class Act {

private:
    uint8_t _id;
    const char* _name;
    const char** _phrases;
    size_t _phraseCount = COUNT_PHRASES;
    const uint16_t* _background;
    int _lastPhraseIndex = -1;
    uint16_t _fontColor;

    size_t getPhraseCount() const;

public:
    Act();
    Act(uint8_t id, const char* name, const char** phrases, uint16_t fontColor, const uint16_t* background);
    uint8_t getId() const;
    const char* getName() const;
    const char* getNewPhrase();
    const uint16_t* getBackground() const;
    uint16_t getFontColor() const;
};

}
