# Clockwise HUB75 Integration

This component provides a hybrid approach that combines the robust HUB75 matrix display support from Stuart Parmenter's [hub75-esphome](https://github.com/stuartparmenter/hub75-esphome) component with clockface functionality and native Home Assistant entity integration.

## Architecture Overview

The hybrid approach leverages the best of both worlds:

- **HUB75 Component**: Handles all low-level display functionality, hardware drivers, multi-panel support, and DMA-based rendering
- **Clockwise Component**: Manages clockface logic and provides native Home Assistant entities
- **Entities**: Native power switch and ESPHome template controls for other settings

## Features

- ✅ **Mature Display Foundation**: Built on tested HUB75 component with comprehensive hardware support
- ✅ **Native HA Entities**: Brightness control, power switch, clockface selection
- ✅ **Multiple Clockfaces**: Pacman and Mario (`CLOCK` is reserved, not implemented)
- ✅ **Hardware Compatibility**: Supports all ESP32 variants, multiple panel layouts, various driver chips
- ✅ **Advanced Features**: Multi-panel support, brightness control, power management

## Special Days

Mario and Pac-Man support recurring special dates entered as `MM-DD:Name,MM-DD:Name` in the **Special Dates (MM-DD:Name,...)** text control. Both scroll `Happy Birthday Name!` from right to left. Mario shows a balloon and uses the top during the day, or warm cream over dark brick ground at night. Pac-Man replaces the date line with a pink greeting clipped to that area and a small red balloon roaming the corridors without consuming food. The date returns and the balloon disappears on ordinary days.

The first matching entry is used, with a maximum of 10 valid entries and 24 bytes per name. The example control persists its text and reapplies it after reboot. See the [Special Days guide](../../README.md#special-days) for setup, limits and examples.

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
    update_interval: 4294967295ms  # Clockwise handles updates
```

### 3. Add Clockwise Component

```yaml
clockwise_hub75:
  id: clockwise_main
  hub75_id: matrix_display
  clockface_type: PACMAN
  initial_brightness: 128
  night_mode_start: "20:00"
  night_mode_end: "06:00"
```

### 4. Add Home Assistant Entities

```yaml
switch:
  - platform: clockwise_hub75
    clockwise_hub75_id: clockwise_main
    name: "Display Power"

```

Use the template brightness, selectors and text/time entities in [the complete example](../../examples/clockwise.yaml) for the remaining controls and persistence. Enable `web_server` for the device web UI, with `ota: false` to keep normal firmware uploads on encrypted ESPHome OTA.

## Configuration Reference

### Clockwise HUB75 Component

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `hub75_id` | ID | **Required** | Reference to HUB75 display component |
| `clockface_type` | enum | `PACMAN` | Initial clockface: `PACMAN`, `MARIO`; `DUNE` is reserved |
| `panel_color_order` | enum | `RGB` | Channel order: `RGB`, `RBG`, `GRB`, `GBR`, `BRG`, `BGR` |
| `special_dates` | string | `""` | Initial recurring dates as `MM-DD:Name,MM-DD:Name` |
| `initial_brightness` | int | `128` | Initial brightness (0-255) |
| `night_mode_start` | time string | `"20:00"` | Initial Mario night-mode start, strictly `HH:MM` |
| `night_mode_end` | time string | `"06:00"` | Initial Mario night-mode end, strictly `HH:MM` |
| `update_interval` | duration | `16ms` | Clockface update interval |

Component options configure startup values, not persistence. Night start/end defaults are shared with the restored template-time entities using substitutions in the example, avoiding separate defaults. Saved UI times override the defaults after reboot. The visible text controls show hours and minutes only; the hidden time entities retain their existing IDs and persistence.

Night start is inclusive and end is exclusive. Overnight and same-day intervals are supported; equal endpoints disable night mode. See the [night-mode persistence guide](../../README.md#night-mode-defaults-and-persistence) and [encrypted OTA setup](../../README.md#encrypted-ota-updates).

### Home Assistant Entities

#### Number Entity (Brightness)

- **Platform**: `template` (with restored globals in the complete example)
- **Range**: 0-255
- **Purpose**: Control display brightness
- **Icon**: `mdi:brightness-5`

#### Switch Entity (Power)

- **Platform**: `clockwise_hub75`
- **Purpose**: Turn display on/off
- **Icon**: `mdi:power`

### Time Source Selection

The component supports three time sources:

1. **Home Assistant Time** (`source = 0`): Requires WiFi + HA connection, automatically synced
2. **NTP (SNTP)** (`source = 1`): Syncs from internet time servers directly, no HA required
3. **Hardware RTC (e.g., BM8563)** (`source = 2`): Maintains time without WiFi, requires initial sync

**Configuration Example:**

```yaml
time:
  - platform: homeassistant
    id: homeassistant_time
  - platform: sntp
    id: ntp_time
    servers:
      - 0.pool.ntp.org
      - 1.pool.ntp.org
      - 2.pool.ntp.org
  - platform: bm8563
    id: rtc_time
    address: 0x51

esphome:
  on_boot:
    priority: 600
    then:
      - lambda: |-
          id(clockwise_main).set_ha_time(id(homeassistant_time));
          id(clockwise_main).set_ntp_time(id(ntp_time));
          id(clockwise_main).set_rtc_time(id(rtc_time));

select:
  - platform: template
    id: time_source_selector
    name: "Time Source"
    options:
      - "Home Assistant"
      - "NTP"
      - "RTC"
    initial_option: "Home Assistant"
    restore_value: true
    optimistic: true
    on_value:
      - lambda: |-
          // 0 = Home Assistant, 1 = NTP, 2 = RTC
          int source = (x == "Home Assistant") ? 0 : (x == "NTP") ? 1 : 2;
          id(clockwise_main).set_time_source(source);

button:
  - platform: template
    name: "Sync Time to RTC"
    icon: mdi:clock-check
    on_press:
      - bm8563.write_time:
          id: rtc_time
```

**Benefits:**
- **NTP**: Accurate time from public servers without needing HA — great for standalone deployments
- **RTC**: Clock continues working during WiFi outages
- One-click sync button to update RTC from Home Assistant
- Switchable at runtime without recompilation

#### Select Entity (Clockface)

- **Platform**: `template`
- **Options**: "Pacman", "Mario"
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