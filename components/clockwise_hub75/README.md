# Clockwise HUB75 Integration

This component provides a hybrid approach that combines the robust HUB75 matrix display support from Stuart Parmenter's [hub75-esphome](https://github.com/stuartparmenter/hub75-esphome) component with clockface functionality and native Home Assistant entity integration.

## Architecture Overview

The hybrid approach leverages the best of both worlds:

- **HUB75 Component**: Handles all low-level display functionality, hardware drivers, multi-panel support, and DMA-based rendering
- **Clockwise Component**: Manages clockface logic and provides native Home Assistant entities
- **Native Entities**: Number, Switch, and Select entities for seamless HA integration

## Features

- ✅ **Mature Display Foundation**: Built on tested HUB75 component with comprehensive hardware support
- ✅ **Native HA Entities**: Brightness control, power switch, clockface selection
- ✅ **Multiple Clockfaces**: Pacman, Mario, and basic clock modes
- ✅ **Hardware Compatibility**: Supports all ESP32 variants, multiple panel layouts, various driver chips
- ✅ **Advanced Features**: Multi-panel support, brightness control, power management

## Quick Start

### 1. Add External Components

```yaml
external_components:
  # Stuart Parmenter's HUB75 component
  - source: github://stuartparmenter/hub75-esphome
    components: [hub75]
  
  # Clockwise integration component
  - source: 
      type: local
      path: components
    components: [clockwise_hub75]
```

### 2. Configure HUB75 Display

```yaml
display:
  - platform: hub75
    id: matrix_display
    panel_width: 64
    panel_height: 32
    brightness: 128
    update_interval: never  # Clockwise handles updates
```

### 3. Add Clockwise Component

```yaml
clockwise_hub75:
  id: clockwise_main
  display_id: matrix_display
  clockface_type: PACMAN
  initial_brightness: 128
```

### 4. Add Home Assistant Entities

```yaml
number:
  - platform: clockwise_hub75
    clockwise_hub75_id: clockwise_main
    name: "Display Brightness"

switch:
  - platform: clockwise_hub75
    clockwise_hub75_id: clockwise_main
    name: "Display Power"

select:
  - platform: clockwise_hub75
    clockwise_hub75_id: clockwise_main
    name: "Clockface"
```

## Configuration Reference

### Clockwise HUB75 Component

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `display_id` | ID | **Required** | Reference to HUB75 display component |
| `clockface_type` | enum | `PACMAN` | Initial clockface: `PACMAN`, `MARIO`, `DUNE` |
| `initial_brightness` | int | `128` | Initial brightness (0-255) |

### Home Assistant Entities

#### Number Entity (Brightness)

- **Range**: 0-255
- **Purpose**: Control display brightness
- **Icon**: `mdi:brightness-5`

#### Switch Entity (Power)

- **Purpose**: Turn display on/off
- **Icon**: `mdi:power`

#### Select Entity (Clockface)

- **Options**: "Pacman", "Mario", "Clock"
- **Purpose**: Switch between clockface modes
- **Icon**: `mdi:clock-outline`

## Hardware Support

This component inherits all hardware support from the HUB75 component:

- **ESP32 Variants**: ESP32, ESP32-S2, ESP32-S3, ESP32-C6, ESP32-P4
- **Driver Chips**: Generic, FM6126A, ICN2038S, FM6124, MBI5124, DP3246
- **Panel Layouts**: Single panel, horizontal chains, 2D grids with zigzag support
- **Scan Patterns**: 1/16 scan, 1/32 scan, 1/4 scan variants

## Migration from Existing Components

### From Native Clock Components

If you have existing `clockwise_clock_ext` components:

1. Replace the display platform with HUB75
2. Update component references to `clockwise_hub75`
3. Enjoy enhanced hardware support and stability

### Adding Multi-Panel Support

The HUB75 foundation makes multi-panel setups trivial:

```yaml
display:
  - platform: hub75
    id: matrix_display
    panel_width: 64
    panel_height: 32
    layout_cols: 2  # 2 panels horizontally  
    layout_rows: 1
    layout: HORIZONTAL
```

## Development Roadmap

### Phase 1: Core Integration ✅

- [x] HUB75 component integration
- [x] Native HA entities  
- [x] Basic clockface switching
- [x] Brightness and power control

### Phase 2: Clockface Migration

- [x] Port existing Pacman clockface logic
- [x] Port existing Mario clockface logic  
- [ ] Implement enhanced clock mode
- [ ] Add font rendering support

### Phase 3: Advanced Features

- [ ] Animation support
- [ ] Custom clockface API
- [ ] Multi-panel clockface layouts
- [ ] LVGL integration option

## Examples

See the `examples/` directory for complete configuration examples:

- `clockwise_hub75_example.yaml` - Basic single panel setup
- `clockwise_hub75_multi_panel.yaml` - Multi-panel configuration (planned)

## Contributing

This component builds on the excellent work by:

- Stuart Parmenter ([hub75-esphome](https://github.com/stuartparmenter/hub75-esphome))
- ESPHome community
- Original clockwise component developers

## License

MIT License - see LICENSE file for details.

## Support

For issues related to:

- **HUB75 hardware/drivers**: Report to [hub75-esphome](https://github.com/stuartparmenter/hub75-esphome) 
- **Clockwise integration**: Report to this repository
- **ESPHome core**: Report to [ESPHome](https://github.com/esphome/esphome)