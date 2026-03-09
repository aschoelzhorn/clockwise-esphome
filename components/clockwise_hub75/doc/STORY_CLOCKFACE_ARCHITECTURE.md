# Story Clockface — Generic Multi-Theme Architecture

## Overview

This document proposes a refactored architecture that separates **generic story-based clockface logic** from **theme-specific content**, enabling support for multiple themes (Dune, Star Trek, Wednesday, etc.) with minimal code duplication.

---

## Core Principle

**One generic engine. Many themes.**

```
Generic Engine (Story Orchestration + Rendering)
        ↓
    Theme Interface
    ↙         ↓         ↖
 Dune    Star Trek    Wednesday
```

---

## Generic Components (Theme-Agnostic)

### 1. **IStoryTheme** (Interface)

Base class that all story themes inherit from:

```cpp
class IStoryTheme {
public:
  virtual ~IStoryTheme() = default;

  // Configuration
  virtual uint8_t getActCount() const = 0;
  virtual uint32_t getActDurationSeconds() const = 0;
  virtual const char* getThemeName() const = 0;

  // Act management
  virtual Class* getAct(uint8_t actId) const = 0;
  virtual uint8_t getCurrentActId(const ESPTime& now) const;

  // Rendering
  virtual void renderAmbientEffect(
    GFXWrapper* gfx,
    uint8_t actId,
    uint32_t frameCount
  ) = 0;

  virtual void renderBackground(GFXWrapper* gfx, uint8_t actId) = 0;
  virtual void renderTimeDisplay(
    GFXWrapper* gfx,
    uint8_t actId,
    const ESPTime& now
  ) = 0;

  virtual void renderTextField(
    GFXWrapper* gfx,
    uint8_t actId,
    const char* text
  ) = 0;
};
```

### 2. **StoryClockface** (Generic Orchestrator)

Main engine that drives all story-based clockfaces:

```cpp
class StoryClockface : public IClockface {
private:
  IStoryTheme* theme_;
  uint8_t currentActId_;
  uint32_t frameCount_;
  const char* currentPhrase_;
  ESPTime lastTime_;

public:
  StoryClockface(IStoryTheme* theme);
  
  void setup(CWDateTime* dt) override;
  void render(GFXWrapper* gfx, const ESPTime& now) override;

private:
  void updateAct(const ESPTime& now);
  void updatePhrase(uint8_t actId);
  void renderFrame(GFXWrapper* gfx, const ESPTime& now);
};
```

**Rendering pipeline (universal for all themes):**

```cpp
void StoryClockface::renderFrame(GFXWrapper* gfx, const ESPTime& now) {
  // Layer 0: Clear
  gfx->clear();

  // Layer 1: Background (theme-specific)
  theme_->renderBackground(gfx, currentActId_);

  // Layer 2: Ambient effects (theme-specific)
  theme_->renderAmbientEffect(gfx, currentActId_, frameCount_);

  // Layer 3: Major events (theme-specific)
  // (placeholder for future event system)

  // Layer 4: Time display (theme-specific positioning)
  theme_->renderTimeDisplay(gfx, currentActId_, now);

  // Layer 5: Text overlay (theme-specific)
  theme_->renderTextField(gfx, currentActId_, currentPhrase_);

  frameCount_++;
}
```

### 3. **Act** (Generic Data Container)

Encapsulates per-act data (reused by all themes):

```cpp
class Act {
private:
  uint8_t id_;
  const char* name_;
  const char** phrases_;
  size_t phraseCount_;
  const uint16_t* backgroundImage_;
  uint16_t fontColor_;
  int lastPhraseIndex_;

public:
  Act(uint8_t id, const char* name, const char** phrases,
      size_t phraseCount, const uint16_t* bg, uint16_t color);

  uint8_t getId() const { return id_; }
  const char* getName() const { return name_; }
  const char* getNewPhrase();
  const uint16_t* getBackground() const { return backgroundImage_; }
  uint16_t getFontColor() const { return fontColor_; }
};
```

### 4. **PhraseManager** (Generic Selection Logic)

Handles phrase selection logic (prevents repetition, etc.):

```cpp
class PhraseManager {
public:
  static const char* selectPhrase(
    const char** phrases,
    size_t count,
    int& lastIndex
  );
};
```

---

## Theme-Specific Components

Each theme is a self-contained module implementing `IStoryTheme`:

### **Dune Theme**

```
theme/dune/
├── dune_theme.h
├── dune_theme.cpp          ← Implements IStoryTheme
├── dune_act_config.h       ← Act definitions (6 acts)
├── dune_phrases.h          ← Phrase pools
├── dune_assets.h           ← Background images, colors
└── dune_ambient.h/cpp      ← Ambient effect implementations
```

**Example implementation:**

```cpp
// dune_theme.h
class DuneTheme : public IStoryTheme {
private:
  Act acts_[6];  // 6 acts for Dune

public:
  DuneTheme();
  uint8_t getActCount() const override { return 6; }
  uint32_t getActDurationSeconds() const override { return 4 * 3600; }
  const char* getThemeName() const override { return "Dune"; }

  Act* getAct(uint8_t actId) const override;
  void renderAmbientEffect(GFXWrapper* gfx, uint8_t actId, uint32_t frame) override;
  void renderBackground(GFXWrapper* gfx, uint8_t actId) override;
  void renderTimeDisplay(GFXWrapper* gfx, uint8_t actId, const ESPTime& now) override;
  void renderTextField(GFXWrapper* gfx, uint8_t actId, const char* text) override;
};
```

### **Star Trek Theme**

```
theme/startrek/
├── startrek_theme.h
├── startrek_theme.cpp       ← Implements IStoryTheme
├── startrek_act_config.h    ← Act definitions (e.g., 4 or 6 acts)
├── startrek_phrases.h       ← Federation/Starfleet quotes
├── startrek_assets.h        ← Starfield, ship visuals, colors
└── startrek_ambient.h/cpp   ← Ambient effects (starfield drift, etc.)
```

**Key difference from Dune:**

```cpp
class StarTrekTheme : public IStoryTheme {
private:
  Act acts_[4];  // Different number!

public:
  uint8_t getActCount() const override { return 4; }
  uint32_t getActDurationSeconds() const override { return 6 * 3600; }  // Different duration!
  const char* getThemeName() const override { return "Star Trek"; }
  // ... rest of implementation
};
```

### **Wednesday Theme**

```
theme/wednesday/
├── wednesday_theme.h
├── wednesday_theme.cpp
├── wednesday_act_config.h
├── wednesday_phrases.h
├── wednesday_assets.h
└── wednesday_ambient.h/cpp
```

---

## File Organization

```
components/clockwise_hub75/
├── Generic Core (Theme-Agnostic)
│   ├── IClockface.h              ← Base interface for all clockfaces
│   ├── IStoryTheme.h             ← Theme interface
│   ├── StoryClockface.h/cpp      ← Generic orchestrator
│   ├── Act.h/cpp                 ← Generic Act container
│   └── PhraseManager.h/cpp       ← Phrase selection logic
│
├── Existing Clockfaces
│   ├── mario_Clockface.h/cpp
│   ├── pacman_Clockface.h/cpp
│   └── (non-story-based)
│
└── Story-Based Themes
    ├── theme/dune/
    │   ├── dune_theme.h/cpp
    │   ├── dune_act_config.h
    │   ├── dune_phrases.h
    │   ├── dune_assets.h
    │   ├── dune_ambient.h/cpp
    │   ├── dune_story.md         ← Documentation
    │   ├── dune_visuals.md
    │   ├── dune_ambient_effects.md
    │   └── dune_ARCHITECTURE.md
    │
    ├── theme/startrek/
    │   ├── startrek_theme.h/cpp
    │   ├── startrek_act_config.h
    │   ├── startrek_phrases.h
    │   ├── startrek_assets.h
    │   ├── startrek_ambient.h/cpp
    │   └── (documentation)
    │
    └── theme/wednesday/
        ├── wednesday_theme.h/cpp
        ├── wednesday_act_config.h
        ├── wednesday_phrases.h
        ├── wednesday_assets.h
        ├── wednesday_ambient.h/cpp
        └── (documentation)
```

---

## Data Flow

```
Current Time (00-24 hours)
        ↓
    StoryClockface::render()
        ↓
    IStoryTheme::getCurrentActId()
        ↓
    Get Act from theme
        ↓
    ┌─────────────────────────────────┐
    │   Generic Rendering Pipeline    │
    │                                 │
    │  Call theme-specific renderers: │
    │  • renderBackground()           │
    │  • renderAmbientEffect()        │
    │  • renderTimeDisplay()          │
    │  • renderTextField()            │
    │                                 │
    └─────────────────────────────────┘
        ↓
    HUB75 Display
```

---

## Adding a New Theme (Step-by-Step)

### Step 1: Define Act Count & Duration

```cpp
// wednesday_theme.h
class WednesdayTheme : public IStoryTheme {
public:
  uint8_t getActCount() const override { return 3; }
  uint32_t getActDurationSeconds() const override { return 8 * 3600; }
  const char* getThemeName() const override { return "Wednesday Addams"; }
};
```

### Step 2: Create Phase/Act Definitions

```cpp
// wednesday_act_config.h
const char* wednesday_act_names[] = {
  "The Mansion",
  "Nevermore Academy",
  "The Mystery"
};

// Color palette
uint16_t wednesday_colors[] = {
  RGB565(0, 0, 0),        // Black
  RGB565(70, 70, 70),    // Gray
  RGB565(150, 150, 150), // Light gray
};
```

### Step 3: Create Phrase Pools

```cpp
// wednesday_phrases.h
const char* wednesday_act0_phrases[] = {
  "Things are rotten",
  "In this world",
  "Pain is inevitable",
  // ...
};

const char** wednesday_phrase_pools[] = {
  wednesday_act0_phrases,
  wednesday_act1_phrases,
  wednesday_act2_phrases,
};

size_t wednesday_phrase_counts[] = {
  sizeof(wednesday_act0_phrases) / sizeof(const char*),
  // ...
};
```

### Step 4: Create Background Images

```cpp
// wednesday_assets.h
const uint16_t wednesday_bg_mansion[64*64] = { /* ... */ };
const uint16_t wednesday_bg_academy[64*64] = { /* ... */ };
const uint16_t wednesday_bg_mystery[64*64] = { /* ... */ };

const uint16_t* wednesday_backgrounds[] = {
  wednesday_bg_mansion,
  wednesday_bg_academy,
  wednesday_bg_mystery,
};
```

### Step 5: Implement Ambient Effects

```cpp
// wednesday_ambient.h/cpp
void WednesdayTheme::renderAmbientEffect(
  GFXWrapper* gfx,
  uint8_t actId,
  uint32_t frame
) {
  switch (actId) {
    case 0: renderMansionEffect(gfx, frame); break;
    case 1: renderAcademyEffect(gfx, frame); break;
    case 2: renderMysteryEffect(gfx, frame); break;
  }
}

void WednesdayTheme::renderMansionEffect(GFXWrapper* gfx, uint32_t frame) {
  // Slow candle flicker or decay effect
  int flicker = (frame / 10) % 4;
  // Modulate brightness accordingly
}
```

### Step 6: Implement Theme Class

```cpp
// wednesday_theme.cpp
WednesdayTheme::WednesdayTheme() {
  acts_[0] = Act(0, "The Mansion", wednesday_act0_phrases, 
                 wednesday_phrase_counts[0], wednesday_bg_mansion, 
                 wednesday_colors[0]);
  acts_[1] = Act(1, "Nevermore Academy", wednesday_act1_phrases,
                 wednesday_phrase_counts[1], wednesday_bg_academy,
                 wednesday_colors[1]);
  acts_[2] = Act(2, "The Mystery", wednesday_act2_phrases,
                 wednesday_phrase_counts[2], wednesday_bg_mystery,
                 wednesday_colors[2]);
}

void WednesdayTheme::renderBackground(GFXWrapper* gfx, uint8_t actId) {
  if (actId < 3) {
    gfx->drawRGB565Image(0, 0, 64, 64, wednesday_backgrounds[actId]);
  }
}

void WednesdayTheme::renderTimeDisplay(
  GFXWrapper* gfx,
  uint8_t actId,
  const ESPTime& now
) {
  // Position and render HH:MM with appropriate font color
  uint16_t color = wednesday_colors[actId];
  // Render time centered at specific position
}

void WednesdayTheme::renderTextField(
  GFXWrapper* gfx,
  uint8_t actId,
  const char* text
) {
  // Render phrase text
  uint16_t color = wednesday_colors[actId];
  // Position text appropriately
}
```

### Step 7: Register Theme

In `clockwise_hub75.cpp`:

```cpp
switch (clockface_type) {
  case PACMAN:
    clockface_ = new pacman::Clockface(gfx_wrapper_);
    break;
  case MARIO:
    clockface_ = new mario::Clockface(gfx_wrapper_);
    break;
  case CLOCK_DUNE:
    theme_ = new DuneTheme();
    clockface_ = new StoryClockface(theme_);
    break;
  case CLOCK_STARTREK:
    theme_ = new StarTrekTheme();
    clockface_ = new StoryClockface(theme_);
    break;
  case CLOCK_WEDNESDAY:
    theme_ = new WednesdayTheme();
    clockface_ = new StoryClockface(theme_);
    break;
}
```

---

## Benefits of This Approach

✅ **Zero code duplication** – Generic engine shared by all story themes  
✅ **Easy to add themes** – Implement `IStoryTheme` interface  
✅ **Flexible act counts** – Each theme defines its own number of acts  
✅ **Flexible act duration** – Each theme defines its own time division  
✅ **Isolated changes** – Modifying Dune doesn't affect Star Trek  
✅ **Testable** – Mock themes for unit testing  
✅ **Scalable** – Add 10 themes without touching core engine  

---

## Migration Path from Current Dune Implementation

**Current state:**
- `DuneClockface` extends `IClockface` directly
- Hardcoded 6 acts, 4-hour duration
- Rendering logic mixed with Dune specifics

**New state:**
- `DuneTheme` implements `IStoryTheme`
- `StoryClockface` handles generic orchestration
- Dune rendering logic moved to `DuneTheme` methods

**Migration steps:**
1. Extract generic rendering logic from `DuneClockface` → `StoryClockface`
2. Create `DuneTheme` class implementing `IStoryTheme`
3. Move Dune-specific render methods to `DuneTheme`
4. Create theme/ directory structure
5. Update `clockwise_hub75.cpp` to instantiate themes
6. Keep backward compatibility: `DuneClockface` can wrap `StoryClockface(new DuneTheme())`

---

## References

- **Generic Core**: `IStoryTheme.h`, `StoryClockface.h/cpp`, `Act.h/cpp`
- **Dune Theme**: `theme/dune/dune_theme.h/cpp`, `dune_ARCHITECTURE.md`
- **Star Trek Theme**: `theme/startrek/startrek_theme.h/cpp`
- **Wednesday Theme**: `theme/wednesday/wednesday_theme.h/cpp`

---

## Questions to Resolve

1. **Memory** – Are all theme assets loaded at startup, or lazy-loaded per theme selection?
2. **Font system** – Should fonts be theme-configurable or global?
3. **Layout system** – Should time/text positioning be part of theme interface?
4. **Event system** – How do "major events" (ornithopter, worm, etc.) scale to other themes?
5. **Compile time** – Should themes be optional (conditional compilation)?

