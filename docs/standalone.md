# Standalone firmware

This public firmware needs no Home Assistant, ESPHome Desktop pairing, personal
Wi-Fi credentials, or shared API/OTA key. Wi-Fi is configured on the device.
Mario and Pac-Man, brightness, panel color order, night times, and special dates
are controlled through its locally hosted web UI.

## Supported hardware and defaults

- Huidu HD-WF2 with ESP32-S3 and at least 4 MB flash; no PSRAM required.
- One 64x64 FM6126A HUB75 panel, standard two-scan wiring.
- Mario clockface, RGB color order, brightness 128, rotation 0.
- NTP time by default; the onboard BM8563 RTC is updated after NTP sync.
- Central European time (`Europe/Vienna`), with automatic daylight saving.
- Night mode 20:00-06:00; endpoints and special dates are editable and persistent.

The 4 MB flash layout also works on the usual 8 MB HD-WF2 boards. This image is
not a universal image for other boards, panel dimensions, or driver chips.
Change the source YAML and rebuild for different hardware, timezone, or rotation.
This standalone image intentionally has no Home Assistant API or native
ESPHome OTA listener. Use the main example for a personalized HA/encrypted-OTA build.

## Download contents

- `firmware.factory.bin`: complete image for initial USB installation.
- `firmware.ota.bin`: application image for recovery-hotspot updates **after**
  ESPHome is installed.
- `clockwise-standalone.yaml`: public build configuration.
- `SHA256SUMS.txt`: checksums to verify the downloaded binaries.
- `BUILD.txt`: build version and source revision.
- `source`: corresponding Clockwise sources, generated ESPHome runtime sources,
  Adafruit library sources, and build YAML.
- `licenses`: project and dependency license notices.

The included YAML uses local components. Rebuild from the archive's `source`
directory or a checkout of the full repository, not just the top-level YAML.

## First installation from original Clockwise firmware

Use USB for the first migration. Original Clockwise firmware is not ESPHome;
its browser uploader and flash partitions are not assumed compatible.
**Do not upload the factory image through the original firmware's browser UI.**

1. Connect the HD-WF2's USB data port to a PC with a data-capable cable.
2. Identify the serial port, for example `COM5` on Windows.
3. Use esptool to erase flash and write the factory image at address `0x0`:

   ```powershell
   python -m esptool --chip esp32s3 --port COM5 erase-flash
   python -m esptool --chip esp32s3 --port COM5 write-flash 0x0 .\firmware.factory.bin
   ```

   Replace `COM5` with the actual port. **Erasing flash deletes the old firmware
   and all saved settings.** Keep a backup if you need to restore the old image.
   If necessary, hold BOOT while connecting/resetting the board to enter its
   serial download mode.

   ESPHome Web at <https://web.esphome.io/> is another USB flashing option in a
   supported Chromium browser. Select the factory image and erase the device
   when migrating from original firmware.

4. Reset the board and keep its normal panel power connected.

Flashing instructions are provided for the recipient; creating this download
does not flash the developer's device.

## Wi-Fi onboarding and everyday use

1. On first boot, wait approximately one minute for an open Wi-Fi network named
   `clockwise-standalone-xxxxxx`, with a device-specific MAC suffix.
2. Join that network from a phone or PC. If its setup page does not open
   automatically, visit <http://192.168.4.1/>.
3. Choose the recipient's own 2.4 GHz Wi-Fi network and enter its password.
   The device stores those credentials locally and connects to that network.
4. Find the clock's IP address in the router's client list, then open
   `http://<clock-ip>/`. The hostname is `clockwise-standalone-xxxxxx.local`
   on networks with working mDNS.
5. Let NTP synchronize. Internet access is needed for initial accurate NTP time;
   the RTC provides time after it has been synchronized and while offline.

Use **Time Source** to choose NTP or RTC. **Sync Time to RTC** manually copies
the current system time to the hardware RTC; do this only after time is correct.
The clock does not reboot just because Home Assistant or Wi-Fi is unavailable.

Brightness, clockface, panel color order, time source, night times, and special
dates survive reboot. Allow ESPHome's default 60-second flash-write interval
before cutting power after an edit.

The timezone is fixed in the supplied binary. For another timezone, edit the
`timezone` substitution and rebuild; do not compensate by selecting incorrect
night-mode times.

## Later firmware updates and security

There is no shared update password/key and no unauthenticated native OTA port.
Normal LAN browser firmware uploads are disabled with `web_server.ota: false`.
USB factory flashing remains available.

For an application-only update, the captive recovery portal supports
`firmware.ota.bin` once ESPHome is installed:

1. Make the saved Wi-Fi network unavailable, for example by turning off its AP
   temporarily, and wait for the clock's fallback hotspot.
2. Connect directly to that hotspot and open <http://192.168.4.1/>.
3. Use the portal's firmware upload to install the application image for this
   exact board/panel. Do not use the factory image here.
4. Restore the normal Wi-Fi network and let the clock reconnect.

The recovery hotspot is deliberately open for onboarding and recovery.
Anyone nearby while it is active can configure Wi-Fi or upload firmware.
Configure it promptly in a trusted environment; avoid leaving it active in
public places. The everyday HTTP settings UI is also unauthenticated, so use a
trusted LAN or isolated IoT network. This image is not intended for Internet
exposure or hostile/shared networks.

For authenticated encrypted OTA, build a personalized image with a unique
device key using the main example. Never distribute a personal build containing
Wi-Fi credentials or keys.

## Rebuilding

From the repository root on Windows, with ESPHome 2026.11.1:

```powershell
esphome compile .\examples\clockwise-standalone.yaml
```

ESPHome Desktop's bundled Python can also compile it:

```powershell
& "$env:LOCALAPPDATA\io.esphome.builder\python\python.exe" -m esphome compile .\examples\clockwise-standalone.yaml
```

Build output is under
`examples\.esphome\build\clockwise-standalone\.pioenvs\clockwise-standalone`.
Only distribute images from this credential-free configuration.
