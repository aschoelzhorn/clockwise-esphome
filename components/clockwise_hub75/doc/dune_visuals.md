
Below is a **complete visual architecture** you can implement incrementally, without rewriting logic, and without needing new concepts later.

This is **not art direction**, this is **render logic**.

---

# DUNE CLOCK — VISUAL SYSTEM

## 1. Layer Model (Critical)

Every frame is composed in **fixed layers**, drawn in this order:

```
L0  Clear / Night sky
L1  Background image (per act)
L2  Ambient motion (sand / wind / heat)
L3  Major event overlays (storm, worm, flight)
L4  Time (HH:MM)
L5  Text overlay (phrases)
```

Rules:

* Lower layers **never know about higher ones**
* Higher layers may *react* to lower ones (contrast boost)

---

## 2. Background Images (Static, Cheap)

You should treat backgrounds as **state anchors**, not decorations.

### One background per Act (6 total)

Each background is:

* **64×64**
* Static
* Low contrast
* No text baked in

| Act | Background Meaning        |
| --- | ------------------------- |
| I   | Cold dunes, empty horizon |
| II  | Dunes with spice shimmer  |
| III | Industrial shadow shapes  |
| IV  | Massive dune curvature    |
| V   | Storm-blurred dunes       |
| VI  | Calm, eroded desert       |

**Rule:**

> Backgrounds never animate. Motion lives above them.

---

## 3. Ambient Motion Layer (Always On)

This prevents “still image syndrome”.

### Per-act ambient effect

| Act | Effect               |
| --- | -------------------- |
| I   | Slow heat shimmer    |
| II  | Floating spice motes |
| III | Shadow drift         |
| IV  | Subsurface tremor    |
| V   | Wind-driven sand     |
| VI  | Occasional dust fall |

### Implementation (cheap)

* 1–3 pixels per frame
* Deterministic noise table
* Wrap-around motion

Example:

```c
x = (x + wind_dx) & 63;
y = (y + wind_dy) & 63;
```

---

## 4. Major Event Visuals (Phase-Based)

These are **interrupts**, not loops.

### Event types

* Ornithopter
* Worm sign
* Storm surge

Each event has **phases**, not frames.

---

## 5. Animation Phases (Universal Pattern)

Every animation follows this:

```
PHASE_ENTER → PHASE_ACTIVE → PHASE_EXIT
```

### Why

* No state explosion
* Easy interruption
* Clean timing

---

## 6. Ornithopter (ACT III, II)

### Visual

* 6–8 px silhouette
* High contrast
* No detail

### Phases

| Phase  | Duration |
| ------ | -------- |
| ENTER  | 300 ms   |
| ACTIVE | 800 ms   |
| EXIT   | 300 ms   |

### Motion

* Diagonal pass
* Shadow crosses **over time digits**

Rule:

> Shadows may cross time, never obscure it.

---

## 7. Worm Sign (ACT IV)

### Visual

* Moving curve under sand
* Dark → light → dark

### Phases

| Phase  | Duration |
| ------ | -------- |
| ENTER  | 500 ms   |
| ACTIVE | 1500 ms  |
| EXIT   | 500 ms   |

### Special rule

* Time contrast boosted
* Colon stops blinking

---

## 8. Storm (ACT V)

### Visual

* Noise + horizontal streaks
* Reduced color palette

### Phases

| Phase  | Duration |
| ------ | -------- |
| ENTER  | 800 ms   |
| ACTIVE | 3000 ms  |
| EXIT   | 1200 ms  |

### Rule

* Background desaturates
* Time becomes brightest element

---

## 9. Time Layer Reactions (Glue Logic)

Time is not passive.

| Situation | Reaction                |
| --------- | ----------------------- |
| Storm     | Solid digits, no dither |
| Worm      | Glow outline            |
| Flight    | Shadow pass             |
| Calm      | Etched look             |

---

## 10. Minimal State Machine (Implementation Key)

You do **not** need multiple machines.

```c
enum VisualState {
  VS_IDLE,
  VS_EVENT_ENTER,
  VS_EVENT_ACTIVE,
  VS_EVENT_EXIT
};
```

Each event:

* Registers itself
* Claims the state
* Releases cleanly

No nesting.

---

## 11. Frame Budget (ESP32 Reality)

| Layer      | Cost          |
| ---------- | ------------- |
| Background | 1 blit        |
| Ambient    | ~5 pixels     |
| Event      | ~10–20 pixels |
| Time       | ~300 pixels   |
| Text       | ~80 pixels    |

This fits comfortably.

---

## 12. What You Can Implement *Right Now*

### Step 1

Load one background per act and swap on act change.

### Step 2

Add **ambient motion** per act (even 1 pixel).

### Step 3

Implement **one event** (ornithopter or storm).

Everything else builds on that.
