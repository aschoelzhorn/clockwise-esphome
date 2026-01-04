#include "dune_act.h"
#include "dune_phrases.h"
#include "dune_assets.h"
#include <stddef.h>

static const char *const TAG = "dune act";

namespace dune {

Act::Act(uint8_t id, const char* name, const char** phrases, uint16_t fontColor, const uint16_t* background) {
    _id = id;
    _name = name;
    _phrases = phrases;
    _phraseCount = COUNT_PHRASES;
    _fontColor = fontColor;
    _background = background;
    _lastPhraseIndex = -1;
    _lastPhraseUpdate = 0;
}

Act::Act() : _id(0), _name(nullptr), _phrases(nullptr), _fontColor(0), _background(nullptr), _lastPhraseIndex(-1) {
    // Default constructor  
}


uint8_t Act::getId() const {
    return _id;
}

const char* Act::getName() const {
    return _name;
}

const char* Act::getPhrase(size_t index) const {
    if (index < _phraseCount) {
        return _phrases[index];
    }
    return nullptr;
}

uint16_t Act::getFontColor() const {
    return _fontColor;
}

const char* Act::getPhrase() {
    // Throttle phrase updates to every 10 seconds
    uint32_t now = millis();
    if (now - _lastPhraseUpdate > 10000) {
        int idx = -1;
        if (_phraseCount == 1) {
            idx = 0;
        } else {
            // Try until we get a different index
            do {
                idx = random(_phraseCount);
            } while (idx == _lastPhraseIndex && _phraseCount > 1);
        }
        _lastPhraseIndex = idx;
        _lastPhraseUpdate = now;
        ESP_LOGD(TAG, "Random phrase index %d", _lastPhraseIndex);        
    }
    return getPhrase(_lastPhraseIndex);
}

size_t Act::getPhraseCount() const {
    return _phraseCount;
}

const uint16_t* Act::getBackground() const {
    return _background;
}

}
