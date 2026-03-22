#include "story_act.h"
#include "story_dune_phrases.h"
#include "story_dune_assets.h"
#include <stddef.h>

static const char *const TAG = "dune act";

namespace story {

Act::Act(uint8_t id, const char* name, const char** phrases, uint16_t fontColor, const uint16_t* background, uint32_t durationSeconds) {
    _id = id;
    _name = name;
    _phrases = phrases;
    _phraseCount = COUNT_PHRASES;
    _fontColor = fontColor;
    _background = background;
    _durationSeconds = durationSeconds;
    _lastPhraseIndex = -1;
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

uint16_t Act::getFontColor() const {
    return _fontColor;
}

const char* Act::getNewPhrase() {
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
    ESP_LOGD(TAG, "Random phrase index %d", _lastPhraseIndex);        
    return _phrases[_lastPhraseIndex];
}

size_t Act::getPhraseCount() const {
    return _phraseCount;
}

const uint16_t* Act::getBackground() const {
    return _background;
}

const ActLayout& Act::getLayout() const {
    return _layout;
}

void Act::setLayout(const ActLayout& layout) {
    _layout = layout;
}

uint32_t Act::getDurationSeconds() const {
    return _durationSeconds;
}

void Act::setDurationSeconds(uint32_t seconds) {
    _durationSeconds = seconds;
}

bool Act::addEvent(const StoryEvent& event) {
    if (_eventCount >= STORY_ACT_MAX_EVENTS) {
        return false;
    }

    _events[_eventCount++] = event;
    return true;
}

size_t Act::getEventCount() const {
    return _eventCount;
}

StoryEvent* Act::getEvent(size_t index) {
    if (index >= _eventCount) {
        return nullptr;
    }
    return &_events[index];
}

const StoryEvent* Act::getEvent(size_t index) const {
    if (index >= _eventCount) {
        return nullptr;
    }
    return &_events[index];
}

void Act::resetEvents(uint32_t actStartMs) {
    for (size_t i = 0; i < _eventCount; i++) {
        _events[i].reset(actStartMs);
    }
}

void Act::updateEvents(uint32_t now) {
    for (size_t i = 0; i < _eventCount; i++) {
        _events[i].update(now);
    }
}

size_t Act::collectRenderSprites(uint32_t now, StoryRenderSprite* out, size_t maxSprites) const {
    if (!out || maxSprites == 0) {
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < _eventCount && count < maxSprites; i++) {
        count += _events[i].collectVisibleSprites(now, out + count, maxSprites - count);
    }
    return count;
}

}
