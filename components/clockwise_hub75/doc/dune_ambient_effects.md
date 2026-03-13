# AMBIENT EFFECTS

## Ambient Heat (Act I)

### Visual idea

* Very subtle vertical shimmer
* Slow temporal change
* Only affects sand / bright areas
* No pixel movement (important!)
* Implemented as brightness modulation

This avoids:

* flicker
* tearing
* text distortion
* expensive per-pixel motion

### Design Rules (locked)

* Runs every frame, but updates slowly
* Uses a phase-based sine
* Modulates brightness by ±1–2 RGB steps
* Skips dark pixels (sky stays calm)
* Never touches text color directly (blending handles this)

## Ambient Shadow (ACT III)

### Narrative intent (so we don’t overdo it)

Act III = The Watchers

* The world feels observed
* Something passes overhead
* Not an event, just presence
* Should be noticeable only if you look for it

So:

* No silhouettes
* No full objects
* Just slow, soft, moving darkness

### Visual behavior

* A wide, soft shadow band
* Moves slowly left → right
* Slight vertical drift
* Never covers the entire screen
* Never erases time (time will be redrawn later)