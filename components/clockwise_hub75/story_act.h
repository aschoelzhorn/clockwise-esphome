#pragma once
#include <stdint.h>
#include <stddef.h>
#include "story_dune_phrases.h"

namespace story {

struct ActLayout {
    int8_t   timeX       = 10;
    int8_t   timeY       = 34;
    int8_t   phraseY     = 2;
    uint32_t textFadeMs  = 1000;
    uint32_t textHoldMs  = 3000;
    uint32_t textQuietMs = 1500;
};

class Act {

private:
    uint8_t _id;
    const char* _name;
    const char** _phrases;
    size_t _phraseCount = COUNT_PHRASES;
    const uint16_t* _background;
    int _lastPhraseIndex = -1;
    uint16_t _fontColor;
    ActLayout _layout;
    uint32_t _durationSeconds = 3600;  // Default 1 hour per act

    size_t getPhraseCount() const;

public:
    Act();
    Act(uint8_t id, const char* name, const char** phrases, uint16_t fontColor, const uint16_t* background, uint32_t durationSeconds = 3600);
    uint8_t getId() const;
    const char* getName() const;
    const char* getNewPhrase();
    const uint16_t* getBackground() const;
    uint16_t getFontColor() const;
    const ActLayout& getLayout() const;
    void setLayout(const ActLayout& layout);
    uint32_t getDurationSeconds() const;
    void setDurationSeconds(uint32_t seconds);
};

}
