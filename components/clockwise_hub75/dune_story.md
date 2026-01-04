# DUNE CLOCK — “TIME OF ARRAKIS”

1. The Clock’s Role (canon in your theme)

This device is a House Atreides desert chronometer.
*Built to survive sandstorms
*Powered by spice
*Time is protected, not decorative
*The desert reacts to time, not the other way around

The clock is not telling the story of Paul.
It is telling the story of time itself on Arrakis.

2. Fixed Layout (64×64 safe design)
This never changes — everything animates around it.

┌──────────────────────────┐
│                          │
│     WORLD / DESERT       │
│     (animated layer)     │
│                          │
│        HH:MM             │  ← Center or lower-center
│   (shielded / carved)    │
│                          │
│                          │
└──────────────────────────┘

## Time rules

* Always readable
* Never fully obscured
* During storms: contrast increases
* During calm: blends into environment

3. Story Progression (Real-Time Driven)

The day on Arrakis is divided into 6 narrative phases.
Each phase lasts 4 real hours → clean, predictable, loopable.

Real Time	Act	Narrative Meaning
00–04	I	The Desert Sleeps
04–08	II	Spice Awakens
08–12	III	The Watchers
12–16	IV	The Maker Stirs
16–20	V	Storm of Fate
20–24	VI	Silence & Survival


4. Acts (What the Clock Is “Saying”)

ACT I — The Desert Sleeps (00–04)
Mood: Cold, still, eternal
* Minimal motion
* Dim palette
* Time looks ancient, carved
-> Time exists before people.

ACT II — Spice Awakens (04–08)
Mood: Opportunity
* Spice particles drift
* Slight glow to digits
* Subtle increase in movement
-> Time begins to matter.

ACT III — The Watchers (08–12)
Mood: Control
* Ornithopter shadow passes occasionally
* High contrast
* Time looks monitored
-> Time is observed — and owned.

ACT IV — The Maker Stirs (12–16)
Mood: Awe
* Sand movement increases
* Worm signs beneath surface
* Time remains calm amid danger
-> Even gods move within time.

ACT V — Storm of Fate (16–20)
Mood: Chaos
* Sandstorm effects
* Digits become brightest element
* Reduced background detail
->Time survives all violence.

ACT VI — Silence & Survival (20–24)
Mood: Reflection
* motion slows again
* Cooler tones
* Time looks worn but intact
-> Only what endures understands time.

5. Micro-Narrative (Minute & Second Use)
This prevents the theme from feeling static.

## Every minute
*Single spice flicker
*Grain shift
*Tiny light pulse on digits

## Every 5 minutes
*Dune reshapes slightly
*Wind direction changes

## Every hour
*Major event:
  *Ornithopter pass
  *Worm sign
  *Storm surge
  *Calm reset


Perfect. Let’s lock **(1) Exact pixel layout** so you can *implement immediately* on a **64×64 LED panel**, readable from across the room, and still very “Dune”.

I’ll keep this **implementation-minded**, not hand-wavy.

---

# DUNE CLOCK — PIXEL LAYOUT SPEC

## 1. Primary Rule (do not break this)

> **Time must be readable at 2–3 meters, even during storms.**

Everything else is subordinate.

---

## 2. Recommended Time Format

**HH:MM**

* No seconds (they add noise)
* Colon blinks once per second (life without clutter)

---

## 3. Font Choice (Pixel Reality)

### Best option: **5×7 font, scaled ×2**

Why:

* Native pixel fonts look *ancient* and carved
* Scales cleanly
* Balanced density for 64×64

### Final digit size

* **Digit:** 10×14 pixels
* **Colon:** 4×14 pixels

---

## 4. Exact Placement (Coordinates)

### Clock bounding box

* Width: **(10×4 digits) + 4 colon = 44 px**
* Height: **14 px**

### Centered horizontally

```
x_start = (64 - 44) / 2 = 10
```

### Vertical placement (critical for composition)

```
y_start = 34
```

This leaves:

* Space above for sky / ornithopter
* Space below for sand movement
* Time slightly **below center**, grounded like something carved into the desert

---

## 5. Pixel Map Overview

```
Y = 0 ─────────────── Sky / Heat / Flight
       Ornithopter / Moons / Dust
Y = 20 ────────────── Horizon / Dunes
Y = 34 ────────────── HH:MM  ← TIME LIVES HERE
Y = 48 ────────────── Sand / Worm Signs
Y = 63 ──────────────
```

---

## 6. Visual Treatment of Digits (Very Important)

### Base state (calm)

* Color: **Spice amber**
* Slight dithering (2 tones)
* Looks etched, not glowing

### During danger (storm / worm)

* Digits become:

  * Brighter
  * Higher contrast
  * Solid fill (no dithering)

Narrative rule:

> When the world becomes hostile, time asserts itself.

---

## 7. Colon Behavior (Subtle Storytelling)

* Blinks **once per second**
* During storms:

  * Stops blinking
  * Stays solid

Meaning:

> Even chaos cannot break time — but it can silence its rhythm.

---

## 8. Shield Effect (Optional but Powerful)

To make time feel *protected*:

* Draw a **1-pixel darker outline** around the entire HH:MM block
* Only visible during:

  * Storms
  * Ornithopter passes
  * Worm signs

This reads as:

* Holtzman shield
* Instrument casing
* Survival technology

---

## 9. Color Discipline (LED Panels Need This)

Recommended core colors:

* **Sand dark**
* **Sand light**
* **Spice amber**
* **Shadow brown**
* **Storm gray**

No gradients.
Use **temporal dithering** if needed.



/*
Event priority (highest wins)
*Act change
*Hour change
*Storm / Worm
*Manual wake
*Ignored
If a higher priority event happens:
*Lower one is discarded
*No queueing


Hard rules (do not break)
* Never show text:
  *Within 2s of minute change
  *During colon blink transition
*Never overlap text
*Never repeat same phrase twice in a row


Timing summary (human-friendly)
| Event       | When              |
| ----------- | ----------------- |
| Act change  | Immediately       |
| Hour change | +500 ms           |
| Storm       | Once per storm    |
| Idle        | Max once / 10 min |

 */
