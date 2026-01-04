#include "dune_Act.h"
#include "dune_phrases.h"
#include "dune_assets.h"
#include <stddef.h>

namespace dune {

Act::Act(uint8_t id, const char* name, const char** phrases, size_t phraseCount, const uint16_t* background)
    : _id(id), _name(name), _phrases(phrases), _phraseCount(phraseCount), _background(background) {}

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

const char* Act::getPhrase() {
    // Throttle phrase updates to every 10 seconds
    _lastPhraseUpdate = 0;
    const char* phrase = nullptr;
    uint32_t now = millis();
    if (now - _lastPhraseUpdate > 10000) {
        // TODO don't repeat the same phrase consecutively
        phrase = getPhrase(random(_phraseCount));
        _lastPhraseUpdate = now;
    }
    return phrase;
}

size_t Act::getPhraseCount() const {
    return _phraseCount;
}

const uint16_t* Act::getBackground() const {
    return _background;
}

}
