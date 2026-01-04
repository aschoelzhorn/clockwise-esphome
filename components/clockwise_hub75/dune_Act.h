#pragma once
#include <stdint.h>
#include <stddef.h>

namespace dune {

class Act {

private:
    uint8_t _id;
    const char* _name;
    const char** _phrases;
    size_t _phraseCount;
    const uint16_t* _background;

    uint32_t _lastPhraseUpdate = 0;

public:
    Act(uint8_t id, const char* name, const char** phrases, size_t phraseCount, const uint16_t* background);
    uint8_t getId() const;
    const char* getName() const;
    const char* getPhrase(size_t index) const;
    const char* getPhrase() const;
    size_t getPhraseCount() const;
    const uint16_t* getBackground() const;

};

}
