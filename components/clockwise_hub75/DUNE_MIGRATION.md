# Dune Clockface — Migration to Generic Story Architecture

## Overview

The Dune clockface has been refactored from a standalone implementation (`DuneClockface`) to a theme-based system. This enables support for multiple story-based themes (Star Trek, Wednesday, etc.) without code duplication.

## What Changed

### Before: Dune-Only

```
DuneClockface (extends IClockface)
├── 6 hardcoded acts
├── Rendering logic mixed with Dune specifics
├── Act class with Dune-specific data
└── Phrase selection logic
```

### After: Generic + Dune Theme

```
Generic Core:
├── IStoryTheme (interface)
├── StoryClockface (generic orchestrator)
├── Act (generic container - moved from dune_act.h)
└── Phrase selection (in Act class)

DuneTheme (extends IStoryTheme):
├── DuneTheme class (implements IStoryTheme)
├── Dune-specific rendering (renderAmbientEffect, etc.)
├── 6 acts configured for Dune narrative
└── dune_phrases.h, dune_assets.h (unchanged)
```

## File Changes

### New Files (Generic Core)

| File | Purpose |
|------|---------|
| `IStoryTheme.h` | Interface for all story themes |
| `IStoryTheme.cpp` | Default implementations |
| `StoryClockface.h` | Generic orchestrator |
| `StoryClockface.cpp` | Generic orchestrator implementation |

### New Files (Dune Theme)

| File | Purpose |
|------|---------|
| `dune_theme.h` | DuneTheme class (implements IStoryTheme) |
| `dune_theme.cpp` | DuneTheme implementation |

### Files Moved/Reorganized

| Old File | New Location | Notes |
|----------|--------------|-------|
| `dune_act.h/cpp` | Kept at top level | Now purely generic, no Dune specifics |
| `dune_phrases.h` | Used by `DuneTheme` | Unchanged |
| `dune_assets.h` | Used by `DuneTheme` | Unchanged |
| `dune_Clockface.h/cpp` | Deprecated* | Functionality moved to `DuneTheme` + `StoryClockface` |
| `dune_ambient_effects.md` | Reference | Ambient effects now in `dune_theme.cpp` |

*`dune_Clockface.h/cpp` can remain for backward compatibility as a wrapper around `StoryClockface(new DuneTheme())`

### Updated Files

| File | Changes |
|------|---------|
| `clockwise_hub75.h/cpp` | Add support for theme-based clockfaces |
| `IClockface.h` | May need signature updates |

## Usage

### Old Way (Direct Dune Clockface)

```cpp
#include "dune_Clockface.h"

clockface_ = new dune::Clockface(gfx_wrapper_);
```

### New Way (Generic Story System)

```cpp
#include "StoryClockface.h"
#include "dune_theme.h"

IStoryTheme* theme = new dune::DuneTheme();
clockface_ = new StoryClockface(theme, gfx_wrapper_);
```

### For New Themes

```cpp
#include "StoryClockface.h"
#include "startrek_theme.h"

IStoryTheme* theme = new startrek::StarTrekTheme();
clockface_ = new StoryClockface(theme, gfx_wrapper_);
```

## Migration Checklist

- [ ] Review `IStoryTheme.h` interface
- [ ] Review `StoryClockface.h/cpp` orchestration logic
- [ ] Review `dune_theme.h/cpp` Dune-specific implementation
- [ ] Update `clockwise_hub75.cpp` to support theme creation
- [ ] Update clockface type enum to support new themes
- [ ] Test Dune rendering with new architecture
- [ ] Verify act transitions work correctly
- [ ] Verify phrase selection works correctly
- [ ] Verify ambient effects are called correctly
- [ ] Document how to add new themes
- [ ] Decide: Keep `dune_Clockface.h/cpp` for backward compat or remove?

## Integration Points

### In `clockwise_hub75.h`

```cpp
enum ClockfaceType {
    PACMAN = 0,
    MARIO = 1,
    CLOCK = 2,
    DUNE = 3,     // Story-based
    STARTREK = 4,  // Future
    WEDNESDAY = 5  // Future
};
```

### In `clockwise_hub75.cpp`

```cpp
switch (clockface_type_) {
    case PACMAN:
        clockface_ = new pacman::Clockface(gfx_wrapper_);
        break;
    case MARIO:
        clockface_ = new mario::Clockface(gfx_wrapper_);
        break;
    case DUNE: {
        IStoryTheme* theme = new dune::DuneTheme();
        clockface_ = new StoryClockface(theme, gfx_wrapper_);
        break;
    }
    case STARTREK: {
        IStoryTheme* theme = new startrek::StarTrekTheme();
        clockface_ = new StoryClockface(theme, gfx_wrapper_);
        break;
    }
    // ... etc
}
```

## Design Decisions Made

### 1. Generic `Act` Class

The original `dune_act.h/cpp` was already generic enough. It's now the universal container for act data, used by all themes.

**Decision**: Keep `Act` at top level; it's not Dune-specific.

### 2. Layer Rendering

The generic `StoryClockface` implements the 6-layer rendering pipeline (Clear, Background, Ambient, Events, Time, Text). All themes use the same pipeline.

**Decision**: This avoids code duplication and ensures consistency.

### 3. Ambient Effects

Each theme implements 6 methods: `renderAmbientEffect_I` through `renderAmbientEffect_VI` (or however many acts it has). The generic `StoryClockface` calls `theme->renderAmbientEffect(gfx, actId, frame)`.

**Decision**: Theme controls what the effect looks like; orchestrator controls when to call it.

### 4. Time/Text Positioning

Currently, positioning is part of the theme implementation. `DuneTheme::renderTimeDisplay()` and `DuneTheme::renderTextField()` decide where to draw.

**Decision**: Allows themes full control over layout. Future: could add a layout system if themes share positioning.

## Testing Strategy

### Unit Tests

1. **Theme selection**: Verify `getCurrentActId()` returns correct act for any time
2. **Phrase selection**: Verify phrases don't repeat consecutively
3. **Act transitions**: Verify act changes at correct times

### Integration Tests

1. **Frame rendering**: Verify all 6 layers are called in order
2. **Time display**: Verify HH:MM is correct and visible
3. **Ambient effects**: Verify effects are visible and animate
4. **Phrase cycling**: Verify new phrases appear regularly

### Visual Tests

1. **Dune visuals**: Does it look like the original Dune clockface?
2. **Color accuracy**: Are colors matching the narrative?
3. **Readability**: Is time always readable?
4. **Smoothness**: Is animation smooth without flickering?

## Rollback Plan

If issues arise:

1. `StoryClockface` can be bypassed; just instantiate `dune::Clockface` directly
2. All original Dune files (`dune_Clockface.h/cpp`, `dune_act.h/cpp`, etc.) remain unchanged in functionality
3. New files can be disabled via compile-time flags if needed

## Future Enhancements

1. **Layout system**: Centralized positioning (time X, Y; text X, Y)
2. **Event system**: Support for major events (storms, worms, etc.) in generic pipeline
3. **Theme management**: Runtime theme switching without recompilation
4. **Asset streaming**: Load backgrounds/phrases from SPIFFS instead of flash
5. **Multi-language**: Support for translations per theme

## References

- `STORY_CLOCKFACE_ARCHITECTURE.md` - Generic architecture design
- `dune_ARCHITECTURE.md` - Dune-specific design (still accurate)
- `dune_story.md` - Dune narrative documentation
- `dune_visuals.md` - Dune visual specifications
- `dune_ambient_effects.md` - Dune ambient effect specifications
