#pragma once

// Dune-themed color palette (RGB565 format)
#define DUNE_SAND_COLOR      0xFDA0  // Sandy beige
#define DUNE_SPICE_COLOR     0xFC00  // Orange spice
#define DUNE_NIGHT_COLOR     0x0014  // Dark blue night sky
#define DUNE_ROCK_COLOR      0x7BCF  // Grey rocks
#define DUNE_SUN_COLOR       0xFFE0  // Bright yellow sun

// TODO: Add sprite arrays for Dune-themed elements
// Examples:
// - Sandworm segments
// - Spice deposits
// - Desert ornithopter
// - Fremen sietch entrance
// - Arrakis moons

// Placeholder sprite data (5x5 pixel example)
const uint16_t DUNE_SANDWORM[25] = {
  0x0000, 0x7BCF, 0x7BCF, 0x7BCF, 0x0000,
  0x7BCF, 0xFDA0, 0xFDA0, 0xFDA0, 0x7BCF,
  0x7BCF, 0xFDA0, 0xFC00, 0xFDA0, 0x7BCF,
  0x7BCF, 0xFDA0, 0xFDA0, 0xFDA0, 0x7BCF,
  0x0000, 0x7BCF, 0x7BCF, 0x7BCF, 0x0000
};

// Add more asset definitions here
