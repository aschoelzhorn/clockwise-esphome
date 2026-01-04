// Auto generated code by esphome
// ========== AUTO GENERATED INCLUDE BLOCK BEGIN ===========
#include "esphome.h"
using namespace esphome;
using std::isnan;
using std::min;
using std::max;
using namespace time;
using namespace display;
using namespace number;
using namespace switch_;
using namespace select;
using namespace sensor;
using namespace text_sensor;
using namespace button;
logger::Logger *logger_logger_id;
web_server_base::WebServerBase *web_server_base_webserverbase_id;
captive_portal::CaptivePortal *captive_portal_captiveportal_id;
wifi::WiFiComponent *wifi_wificomponent_id;
mdns::MDNSComponent *mdns_mdnscomponent_id;
esphome::ESPHomeOTAComponent *esphome_esphomeotacomponent_id;
safe_mode::SafeModeComponent *safe_mode_safemodecomponent_id;
api::APIServer *api_apiserver_id;
using namespace api;
preferences::IntervalSyncer *preferences_intervalsyncer_id;
homeassistant::HomeassistantTime *homeassistant_time;
time::SyncTrigger *time_synctrigger_id;
Automation<> *automation_id;
LambdaAction<> *lambdaaction_id;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_2;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_3;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_4;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_5;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_6;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_7;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_8;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_9;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_10;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_11;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_12;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_13;
esp32::ESP32InternalGPIOPin *esp32_esp32internalgpiopin_id_14;
hub75_display::HUB75Display *matrix_display;
clockwise_hub75::ClockwiseHUB75 *clockwise_main;
clockwise_hub75::ClockwiseBrightness *display_brightness;
clockwise_hub75::ClockwiseSwitch *display_power;
clockwise_hub75::ClockwiseSelect *clockface_selector;
wifi_signal::WiFiSignalSensor *wifi_signal_wifisignalsensor_id;
uptime::UptimeSecondsSensor *uptime_uptimesecondssensor_id;
wifi_info::SSIDWiFiInfo *wifi_info_ssidwifiinfo_id;
wifi_info::BSSIDWiFiInfo *wifi_info_bssidwifiinfo_id;
wifi_info::MacAddressWifiInfo *wifi_info_macaddresswifiinfo_id;
wifi_info::IPAddressWiFiInfo *wifi_info_ipaddresswifiinfo_id;
version::VersionTextSensor *version_versiontextsensor_id;
restart::RestartButton *restart_restartbutton_id;
safe_mode::SafeModeButton *safe_mode_safemodebutton_id;
#define yield() esphome::yield()
#define millis() esphome::millis()
#define micros() esphome::micros()
#define delay(x) esphome::delay(x)
#define delayMicroseconds(x) esphome::delayMicroseconds(x)
// ========== AUTO GENERATED INCLUDE BLOCK END ==========="

void setup() {
  // ========== AUTO GENERATED CODE BEGIN ===========
  App.reserve_text_sensor(5);
  App.reserve_switch(1);
  App.reserve_sensor(2);
  App.reserve_select(1);
  App.reserve_number(1);
  App.reserve_button(2);
  // async_tcp:
  //   {}
  // esphome:
  //   name: clockwise-hdwf2
  //   friendly_name: Clockwise HD-WF2 Clock
  //   comment: Clockwise Clock on HD-WF2 Board with FM6126A Driver
  //   platformio_options:
  //     lib_deps:
  //       - adafruit/Adafruit BusIO
  //       - adafruit/Adafruit GFX Library
  //     lib_ldf_mode: chain+
  //     build_flags:
  //       - -Os
  //       - -DCORE_DEBUG_LEVEL=1
  //   min_version: 2025.6.1
  //   build_path: build\clockwise-hdwf2
  //   area: ''
  //   includes: []
  //   libraries: []
  //   name_add_mac_suffix: false
  //   debug_scheduler: false
  App.pre_setup("clockwise-hdwf2", "Clockwise HD-WF2 Clock", "", "Clockwise Clock on HD-WF2 Board with FM6126A Driver", __DATE__ ", " __TIME__, false);
  App.reserve_components(25);
  // time:
  // display:
  // number:
  // switch:
  // select:
  // sensor:
  // text_sensor:
  // button:
  // logger:
  //   id: logger_logger_id
  //   baud_rate: 115200
  //   tx_buffer_size: 512
  //   deassert_rts_dtr: false
  //   task_log_buffer_size: 768
  //   hardware_uart: USB_CDC
  //   level: DEBUG
  //   logs: {}
  logger_logger_id = new logger::Logger(115200, 512);
  logger_logger_id->create_pthread_key();
  logger_logger_id->init_log_buffer(768);
  logger_logger_id->set_log_level(ESPHOME_LOG_LEVEL_DEBUG);
  logger_logger_id->set_uart_selection(logger::UART_SELECTION_USB_CDC);
  logger_logger_id->pre_setup();
  logger_logger_id->set_component_source("logger");
  App.register_component(logger_logger_id);
  // web_server_base:
  //   id: web_server_base_webserverbase_id
  web_server_base_webserverbase_id = new web_server_base::WebServerBase();
  web_server_base_webserverbase_id->set_component_source("web_server_base");
  App.register_component(web_server_base_webserverbase_id);
  // captive_portal:
  //   id: captive_portal_captiveportal_id
  //   web_server_base_id: web_server_base_webserverbase_id
  captive_portal_captiveportal_id = new captive_portal::CaptivePortal(web_server_base_webserverbase_id);
  captive_portal_captiveportal_id->set_component_source("captive_portal");
  App.register_component(captive_portal_captiveportal_id);
  // wifi:
  //   ap:
  //     ssid: clockwise-hdwf2 Fallback
  //     password: !secret 'wifi_ap_password'
  //     id: wifi_wifiap_id
  //     ap_timeout: 1min
  //   id: wifi_wificomponent_id
  //   domain: .local
  //   reboot_timeout: 15min
  //   power_save_mode: LIGHT
  //   fast_connect: false
  //   passive_scan: false
  //   enable_on_boot: true
  //   networks:
  //     - ssid: !secret 'wifi_ssid'
  //       password: !secret 'wifi_password'
  //       id: wifi_wifiap_id_2
  //       priority: 0.0
  //   use_address: clockwise-hdwf2.local
  wifi_wificomponent_id = new wifi::WiFiComponent();
  wifi_wificomponent_id->set_use_address("clockwise-hdwf2.local");
  {
  wifi::WiFiAP wifi_wifiap_id_2 = wifi::WiFiAP();
  wifi_wifiap_id_2.set_ssid("test_network");
  wifi_wifiap_id_2.set_password("test_password");
  wifi_wifiap_id_2.set_priority(0.0f);
  wifi_wificomponent_id->add_sta(wifi_wifiap_id_2);
  }
  {
  wifi::WiFiAP wifi_wifiap_id = wifi::WiFiAP();
  wifi_wifiap_id.set_ssid("clockwise-hdwf2 Fallback");
  wifi_wifiap_id.set_password("test_ap_password");
  wifi_wificomponent_id->set_ap(wifi_wifiap_id);
  }
  wifi_wificomponent_id->set_ap_timeout(60000);
  wifi_wificomponent_id->set_reboot_timeout(900000);
  wifi_wificomponent_id->set_power_save_mode(wifi::WIFI_POWER_SAVE_LIGHT);
  wifi_wificomponent_id->set_fast_connect(false);
  wifi_wificomponent_id->set_passive_scan(false);
  wifi_wificomponent_id->set_enable_on_boot(true);
  wifi_wificomponent_id->set_component_source("wifi");
  App.register_component(wifi_wificomponent_id);
  // mdns:
  //   id: mdns_mdnscomponent_id
  //   disabled: false
  //   services: []
  mdns_mdnscomponent_id = new mdns::MDNSComponent();
  mdns_mdnscomponent_id->set_component_source("mdns");
  App.register_component(mdns_mdnscomponent_id);
  // ota:
  // ota.esphome:
  //   platform: esphome
  //   password: !secret 'ota_password'
  //   id: esphome_esphomeotacomponent_id
  //   version: 2
  //   port: 3232
  esphome_esphomeotacomponent_id = new esphome::ESPHomeOTAComponent();
  esphome_esphomeotacomponent_id->set_port(3232);
  esphome_esphomeotacomponent_id->set_auth_password("test_ota_password");
  esphome_esphomeotacomponent_id->set_component_source("esphome.ota");
  App.register_component(esphome_esphomeotacomponent_id);
  // safe_mode:
  //   id: safe_mode_safemodecomponent_id
  //   boot_is_good_after: 1min
  //   disabled: false
  //   num_attempts: 10
  //   reboot_timeout: 5min
  safe_mode_safemodecomponent_id = new safe_mode::SafeModeComponent();
  safe_mode_safemodecomponent_id->set_component_source("safe_mode");
  App.register_component(safe_mode_safemodecomponent_id);
  if (safe_mode_safemodecomponent_id->should_enter_safe_mode(10, 300000, 60000)) return;
  // api:
  //   reboot_timeout: 0s
  //   id: api_apiserver_id
  //   port: 6053
  //   password: ''
  //   batch_delay: 100ms
  api_apiserver_id = new api::APIServer();
  api_apiserver_id->set_component_source("api");
  App.register_component(api_apiserver_id);
  api_apiserver_id->set_port(6053);
  api_apiserver_id->set_password("");
  api_apiserver_id->set_reboot_timeout(0);
  api_apiserver_id->set_batch_delay(100);
  // substitutions:
  //   device_name: clockwise-hdwf2
  //   friendly_name: Clockwise HD-WF2 Clock
  //   panel_width: '64'
  //   panel_height: '64'
  //   initial_brightness: '128'
  // esp32:
  //   board: esp32-s3-devkitc-1
  //   variant: ESP32S3
  //   framework:
  //     version: 2.0.5
  //     advanced:
  //       ignore_efuse_custom_mac: false
  //     source: ~3.20005.0
  //     platform_version: platformio/espressif32@5.4.0
  //     type: arduino
  //   flash_size: 4MB
  //   cpu_frequency: 160MHZ
  setCpuFrequencyMhz(160);
  // preferences:
  //   id: preferences_intervalsyncer_id
  //   flash_write_interval: 60s
  preferences_intervalsyncer_id = new preferences::IntervalSyncer();
  preferences_intervalsyncer_id->set_write_interval(60000);
  preferences_intervalsyncer_id->set_component_source("preferences");
  App.register_component(preferences_intervalsyncer_id);
  // time.homeassistant:
  //   platform: homeassistant
  //   id: homeassistant_time
  //   timezone: CET-1CEST,M3.5.0,M10.5.0/3
  //   on_time_sync:
  //     - then:
  //         - logger.log:
  //             format: Time synchronized with Home Assistant
  //             level: DEBUG
  //             tag: main
  //             args: []
  //           type_id: lambdaaction_id
  //       automation_id: automation_id
  //       trigger_id: time_synctrigger_id
  //   update_interval: 15min
  homeassistant_time = new homeassistant::HomeassistantTime();
  homeassistant_time->set_timezone("CET-1CEST,M3.5.0,M10.5.0/3");
  time_synctrigger_id = new time::SyncTrigger(homeassistant_time);
  time_synctrigger_id->set_component_source("time");
  App.register_component(time_synctrigger_id);
  automation_id = new Automation<>(time_synctrigger_id);
  lambdaaction_id = new LambdaAction<>([=]() -> void {
      ESP_LOGD("main", "Time synchronized with Home Assistant");
  });
  automation_id->add_actions({lambdaaction_id});
  homeassistant_time->set_update_interval(900000);
  homeassistant_time->set_component_source("homeassistant.time");
  App.register_component(homeassistant_time);
  // external_components:
  //   - source:
  //       url: https:github.com/stuartparmenter/hub75-esphome.git
  //       type: git
  //     refresh: 1d
  //     components: all
  //   - source:
  //       path: ../components
  //       type: local
  //     refresh: 1d
  //     components: all
  // display.hub75:
  //   platform: hub75
  //   id: matrix_display
  //   board: huidu-hd-wf2
  //   panel_width: 64
  //   panel_height: 64
  //   brightness: 128
  //   bit_depth: 8
  //   min_refresh_rate: 120
  //   double_buffer: false
  //   shift_driver: FM6126A
  //   scan_wiring: STANDARD_TWO_SCAN
  //   clock_speed: 20MHZ
  //   clock_phase: false
  //   update_interval: 4294967295ms
  //   auto_clear_enabled: unspecified
  //   r1_pin:
  //     number: 2
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   g1_pin:
  //     number: 6
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_2
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   b1_pin:
  //     number: 10
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_3
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   r2_pin:
  //     number: 3
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_4
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   g2_pin:
  //     number: 7
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_5
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   b2_pin:
  //     number: 11
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_6
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   a_pin:
  //     number: 39
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_7
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   b_pin:
  //     number: 38
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_8
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   c_pin:
  //     number: 37
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_9
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   d_pin:
  //     number: 36
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_10
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   e_pin:
  //     number: 21
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_11
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   lat_pin:
  //     number: 33
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_12
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   oe_pin:
  //     number: 35
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_13
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  //   clk_pin:
  //     number: 34
  //     mode:
  //       output: true
  //       input: false
  //       open_drain: false
  //       pullup: false
  //       pulldown: false
  //     id: esp32_esp32internalgpiopin_id_14
  //     inverted: false
  //     ignore_pin_validation_error: false
  //     ignore_strapping_warning: false
  //     drive_strength: 20.0
  esp32_esp32internalgpiopin_id = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id->set_pin(::GPIO_NUM_2);
  esp32_esp32internalgpiopin_id->set_inverted(false);
  esp32_esp32internalgpiopin_id->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_2 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_2->set_pin(::GPIO_NUM_6);
  esp32_esp32internalgpiopin_id_2->set_inverted(false);
  esp32_esp32internalgpiopin_id_2->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_2->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_3 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_3->set_pin(::GPIO_NUM_10);
  esp32_esp32internalgpiopin_id_3->set_inverted(false);
  esp32_esp32internalgpiopin_id_3->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_3->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_4 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_4->set_pin(::GPIO_NUM_3);
  esp32_esp32internalgpiopin_id_4->set_inverted(false);
  esp32_esp32internalgpiopin_id_4->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_4->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_5 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_5->set_pin(::GPIO_NUM_7);
  esp32_esp32internalgpiopin_id_5->set_inverted(false);
  esp32_esp32internalgpiopin_id_5->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_5->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_6 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_6->set_pin(::GPIO_NUM_11);
  esp32_esp32internalgpiopin_id_6->set_inverted(false);
  esp32_esp32internalgpiopin_id_6->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_6->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_7 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_7->set_pin(::GPIO_NUM_39);
  esp32_esp32internalgpiopin_id_7->set_inverted(false);
  esp32_esp32internalgpiopin_id_7->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_7->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_8 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_8->set_pin(::GPIO_NUM_38);
  esp32_esp32internalgpiopin_id_8->set_inverted(false);
  esp32_esp32internalgpiopin_id_8->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_8->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_9 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_9->set_pin(::GPIO_NUM_37);
  esp32_esp32internalgpiopin_id_9->set_inverted(false);
  esp32_esp32internalgpiopin_id_9->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_9->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_10 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_10->set_pin(::GPIO_NUM_36);
  esp32_esp32internalgpiopin_id_10->set_inverted(false);
  esp32_esp32internalgpiopin_id_10->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_10->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_11 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_11->set_pin(::GPIO_NUM_21);
  esp32_esp32internalgpiopin_id_11->set_inverted(false);
  esp32_esp32internalgpiopin_id_11->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_11->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_12 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_12->set_pin(::GPIO_NUM_33);
  esp32_esp32internalgpiopin_id_12->set_inverted(false);
  esp32_esp32internalgpiopin_id_12->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_12->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_13 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_13->set_pin(::GPIO_NUM_35);
  esp32_esp32internalgpiopin_id_13->set_inverted(false);
  esp32_esp32internalgpiopin_id_13->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_13->set_flags(gpio::Flags::FLAG_OUTPUT);
  esp32_esp32internalgpiopin_id_14 = new esp32::ESP32InternalGPIOPin();
  esp32_esp32internalgpiopin_id_14->set_pin(::GPIO_NUM_34);
  esp32_esp32internalgpiopin_id_14->set_inverted(false);
  esp32_esp32internalgpiopin_id_14->set_drive_strength(::GPIO_DRIVE_CAP_2);
  esp32_esp32internalgpiopin_id_14->set_flags(gpio::Flags::FLAG_OUTPUT);
  matrix_display = new hub75_display::HUB75Display(Hub75Config{
      .panel_width = 64,
      .panel_height = 64,
      .scan_wiring = ::ScanPattern::STANDARD_TWO_SCAN,
      .shift_driver = ::ShiftDriver::FM6126A,
      .pins = Hub75Pins{
      .r1 = static_cast<int8_t>(esp32_esp32internalgpiopin_id->get_pin()),
      .g1 = static_cast<int8_t>(esp32_esp32internalgpiopin_id_2->get_pin()),
      .b1 = static_cast<int8_t>(esp32_esp32internalgpiopin_id_3->get_pin()),
      .r2 = static_cast<int8_t>(esp32_esp32internalgpiopin_id_4->get_pin()),
      .g2 = static_cast<int8_t>(esp32_esp32internalgpiopin_id_5->get_pin()),
      .b2 = static_cast<int8_t>(esp32_esp32internalgpiopin_id_6->get_pin()),
      .a = static_cast<int8_t>(esp32_esp32internalgpiopin_id_7->get_pin()),
      .b = static_cast<int8_t>(esp32_esp32internalgpiopin_id_8->get_pin()),
      .c = static_cast<int8_t>(esp32_esp32internalgpiopin_id_9->get_pin()),
      .d = static_cast<int8_t>(esp32_esp32internalgpiopin_id_10->get_pin()),
      .e = static_cast<int8_t>(esp32_esp32internalgpiopin_id_11->get_pin()),
      .lat = static_cast<int8_t>(esp32_esp32internalgpiopin_id_12->get_pin()),
      .oe = static_cast<int8_t>(esp32_esp32internalgpiopin_id_13->get_pin()),
      .clk = static_cast<int8_t>(esp32_esp32internalgpiopin_id_14->get_pin()),
    },
      .output_clock_speed = ::Hub75ClockSpeed::HZ_20M,
      .min_refresh_rate = 120,
      .double_buffer = false,
      .clk_phase_inverted = false,
      .brightness = 128,
  });
  matrix_display->set_update_interval(4294967295UL);
  matrix_display->set_component_source("display");
  App.register_component(matrix_display);
  matrix_display->set_auto_clear(false);
  // clockwise_hub75:
  //   id: clockwise_main
  //   hub75_id: matrix_display
  //   time_id: homeassistant_time
  //   clockface_type: DUNE
  //   initial_brightness: 128
  //   update_interval: 16ms
  clockwise_main = new clockwise_hub75::ClockwiseHUB75();
  clockwise_main->set_update_interval(16);
  clockwise_main->set_component_source("clockwise_hub75");
  App.register_component(clockwise_main);
  clockwise_main->set_hub75_display(matrix_display);
  clockwise_main->set_time(homeassistant_time);
  clockwise_main->set_clockface_type(clockwise_hub75::DUNE);
  clockwise_main->set_initial_brightness(128);
  // number.clockwise_hub75:
  //   platform: clockwise_hub75
  //   clockwise_hub75_id: clockwise_main
  //   id: display_brightness
  //   name: Display Brightness
  //   icon: mdi:brightness-5
  //   entity_category: config
  //   mode: SLIDER
  //   disabled_by_default: false
  display_brightness = new clockwise_hub75::ClockwiseBrightness();
  App.register_number(display_brightness);
  display_brightness->set_name("Display Brightness");
  display_brightness->set_object_id("display_brightness");
  display_brightness->set_disabled_by_default(false);
  display_brightness->set_icon("mdi:brightness-5");
  display_brightness->set_entity_category(::ENTITY_CATEGORY_CONFIG);
  display_brightness->traits.set_min_value(0);
  display_brightness->traits.set_max_value(255);
  display_brightness->traits.set_step(1);
  display_brightness->traits.set_mode(number::NUMBER_MODE_SLIDER);
  display_brightness->set_component_source("clockwise_hub75.number");
  App.register_component(display_brightness);
  display_brightness->set_clockwise_hub75(clockwise_main);
  // switch.clockwise_hub75:
  //   platform: clockwise_hub75
  //   clockwise_hub75_id: clockwise_main
  //   id: display_power
  //   name: Display Power
  //   icon: mdi:power
  //   entity_category: config
  //   restore_mode: RESTORE_DEFAULT_ON
  //   disabled_by_default: false
  display_power = new clockwise_hub75::ClockwiseSwitch();
  App.register_switch(display_power);
  display_power->set_name("Display Power");
  display_power->set_object_id("display_power");
  display_power->set_disabled_by_default(false);
  display_power->set_icon("mdi:power");
  display_power->set_entity_category(::ENTITY_CATEGORY_CONFIG);
  display_power->set_restore_mode(switch_::SWITCH_RESTORE_DEFAULT_ON);
  display_power->set_component_source("clockwise_hub75.switch");
  App.register_component(display_power);
  display_power->set_clockwise_hub75(clockwise_main);
  // select.clockwise_hub75:
  //   platform: clockwise_hub75
  //   clockwise_hub75_id: clockwise_main
  //   id: clockface_selector
  //   name: Clockface
  //   icon: mdi:clock-outline
  //   entity_category: config
  //   disabled_by_default: false
  clockface_selector = new clockwise_hub75::ClockwiseSelect();
  App.register_select(clockface_selector);
  clockface_selector->set_name("Clockface");
  clockface_selector->set_object_id("clockface");
  clockface_selector->set_disabled_by_default(false);
  clockface_selector->set_icon("mdi:clock-outline");
  clockface_selector->set_entity_category(::ENTITY_CATEGORY_CONFIG);
  clockface_selector->traits.set_options({"Pacman", "Mario", "Clock", "Dune"});
  clockface_selector->set_component_source("clockwise_hub75.select");
  App.register_component(clockface_selector);
  clockface_selector->set_clockwise_hub75(clockwise_main);
  // sensor.wifi_signal:
  //   platform: wifi_signal
  //   name: WiFi Signal
  //   update_interval: 60s
  //   entity_category: diagnostic
  //   disabled_by_default: false
  //   force_update: false
  //   id: wifi_signal_wifisignalsensor_id
  //   unit_of_measurement: dBm
  //   accuracy_decimals: 0
  //   device_class: signal_strength
  //   state_class: measurement
  wifi_signal_wifisignalsensor_id = new wifi_signal::WiFiSignalSensor();
  App.register_sensor(wifi_signal_wifisignalsensor_id);
  wifi_signal_wifisignalsensor_id->set_name("WiFi Signal");
  wifi_signal_wifisignalsensor_id->set_object_id("wifi_signal");
  wifi_signal_wifisignalsensor_id->set_disabled_by_default(false);
  wifi_signal_wifisignalsensor_id->set_entity_category(::ENTITY_CATEGORY_DIAGNOSTIC);
  wifi_signal_wifisignalsensor_id->set_device_class("signal_strength");
  wifi_signal_wifisignalsensor_id->set_state_class(sensor::STATE_CLASS_MEASUREMENT);
  wifi_signal_wifisignalsensor_id->set_unit_of_measurement("dBm");
  wifi_signal_wifisignalsensor_id->set_accuracy_decimals(0);
  wifi_signal_wifisignalsensor_id->set_force_update(false);
  wifi_signal_wifisignalsensor_id->set_update_interval(60000);
  wifi_signal_wifisignalsensor_id->set_component_source("wifi_signal.sensor");
  App.register_component(wifi_signal_wifisignalsensor_id);
  // sensor.uptime:
  //   platform: uptime
  //   name: Uptime
  //   update_interval: 60s
  //   entity_category: diagnostic
  //   disabled_by_default: false
  //   force_update: false
  //   id: uptime_uptimesecondssensor_id
  //   unit_of_measurement: s
  //   icon: mdi:timer-outline
  //   accuracy_decimals: 0
  //   device_class: duration
  //   state_class: total_increasing
  //   type: seconds
  uptime_uptimesecondssensor_id = new uptime::UptimeSecondsSensor();
  App.register_sensor(uptime_uptimesecondssensor_id);
  uptime_uptimesecondssensor_id->set_name("Uptime");
  uptime_uptimesecondssensor_id->set_object_id("uptime");
  uptime_uptimesecondssensor_id->set_disabled_by_default(false);
  uptime_uptimesecondssensor_id->set_icon("mdi:timer-outline");
  uptime_uptimesecondssensor_id->set_entity_category(::ENTITY_CATEGORY_DIAGNOSTIC);
  uptime_uptimesecondssensor_id->set_device_class("duration");
  uptime_uptimesecondssensor_id->set_state_class(sensor::STATE_CLASS_TOTAL_INCREASING);
  uptime_uptimesecondssensor_id->set_unit_of_measurement("s");
  uptime_uptimesecondssensor_id->set_accuracy_decimals(0);
  uptime_uptimesecondssensor_id->set_force_update(false);
  uptime_uptimesecondssensor_id->set_update_interval(60000);
  uptime_uptimesecondssensor_id->set_component_source("uptime.sensor");
  App.register_component(uptime_uptimesecondssensor_id);
  // text_sensor.wifi_info:
  //   platform: wifi_info
  //   ip_address:
  //     name: IP Address
  //     entity_category: diagnostic
  //     disabled_by_default: false
  //     id: wifi_info_ipaddresswifiinfo_id
  //     update_interval: 1s
  //   ssid:
  //     name: Connected SSID
  //     entity_category: diagnostic
  //     disabled_by_default: false
  //     id: wifi_info_ssidwifiinfo_id
  //     update_interval: 1s
  //   mac_address:
  //     name: Mac Address
  //     entity_category: diagnostic
  //     disabled_by_default: false
  //     id: wifi_info_macaddresswifiinfo_id
  //   bssid:
  //     name: Connected BSSID
  //     entity_category: diagnostic
  //     disabled_by_default: false
  //     id: wifi_info_bssidwifiinfo_id
  //     update_interval: 1s
  wifi_info_ssidwifiinfo_id = new wifi_info::SSIDWiFiInfo();
  App.register_text_sensor(wifi_info_ssidwifiinfo_id);
  wifi_info_ssidwifiinfo_id->set_name("Connected SSID");
  wifi_info_ssidwifiinfo_id->set_object_id("connected_ssid");
  wifi_info_ssidwifiinfo_id->set_disabled_by_default(false);
  wifi_info_ssidwifiinfo_id->set_entity_category(::ENTITY_CATEGORY_DIAGNOSTIC);
  wifi_info_ssidwifiinfo_id->set_update_interval(1000);
  wifi_info_ssidwifiinfo_id->set_component_source("wifi_info.text_sensor");
  App.register_component(wifi_info_ssidwifiinfo_id);
  wifi_info_bssidwifiinfo_id = new wifi_info::BSSIDWiFiInfo();
  App.register_text_sensor(wifi_info_bssidwifiinfo_id);
  wifi_info_bssidwifiinfo_id->set_name("Connected BSSID");
  wifi_info_bssidwifiinfo_id->set_object_id("connected_bssid");
  wifi_info_bssidwifiinfo_id->set_disabled_by_default(false);
  wifi_info_bssidwifiinfo_id->set_entity_category(::ENTITY_CATEGORY_DIAGNOSTIC);
  wifi_info_bssidwifiinfo_id->set_update_interval(1000);
  wifi_info_bssidwifiinfo_id->set_component_source("wifi_info.text_sensor");
  App.register_component(wifi_info_bssidwifiinfo_id);
  wifi_info_macaddresswifiinfo_id = new wifi_info::MacAddressWifiInfo();
  App.register_text_sensor(wifi_info_macaddresswifiinfo_id);
  wifi_info_macaddresswifiinfo_id->set_name("Mac Address");
  wifi_info_macaddresswifiinfo_id->set_object_id("mac_address");
  wifi_info_macaddresswifiinfo_id->set_disabled_by_default(false);
  wifi_info_macaddresswifiinfo_id->set_entity_category(::ENTITY_CATEGORY_DIAGNOSTIC);
  wifi_info_macaddresswifiinfo_id->set_component_source("wifi_info.text_sensor");
  App.register_component(wifi_info_macaddresswifiinfo_id);
  wifi_info_ipaddresswifiinfo_id = new wifi_info::IPAddressWiFiInfo();
  App.register_text_sensor(wifi_info_ipaddresswifiinfo_id);
  wifi_info_ipaddresswifiinfo_id->set_name("IP Address");
  wifi_info_ipaddresswifiinfo_id->set_object_id("ip_address");
  wifi_info_ipaddresswifiinfo_id->set_disabled_by_default(false);
  wifi_info_ipaddresswifiinfo_id->set_entity_category(::ENTITY_CATEGORY_DIAGNOSTIC);
  wifi_info_ipaddresswifiinfo_id->set_update_interval(1000);
  wifi_info_ipaddresswifiinfo_id->set_component_source("wifi_info.text_sensor");
  App.register_component(wifi_info_ipaddresswifiinfo_id);
  // text_sensor.version:
  //   platform: version
  //   name: ESPHome Version
  //   entity_category: diagnostic
  //   disabled_by_default: false
  //   icon: mdi:new-box
  //   id: version_versiontextsensor_id
  //   hide_timestamp: false
  version_versiontextsensor_id = new version::VersionTextSensor();
  App.register_text_sensor(version_versiontextsensor_id);
  version_versiontextsensor_id->set_name("ESPHome Version");
  version_versiontextsensor_id->set_object_id("esphome_version");
  version_versiontextsensor_id->set_disabled_by_default(false);
  version_versiontextsensor_id->set_icon("mdi:new-box");
  version_versiontextsensor_id->set_entity_category(::ENTITY_CATEGORY_DIAGNOSTIC);
  version_versiontextsensor_id->set_component_source("version.text_sensor");
  App.register_component(version_versiontextsensor_id);
  version_versiontextsensor_id->set_hide_timestamp(false);
  // button.restart:
  //   platform: restart
  //   name: Restart
  //   entity_category: diagnostic
  //   disabled_by_default: false
  //   id: restart_restartbutton_id
  //   icon: mdi:restart
  //   device_class: restart
  restart_restartbutton_id = new restart::RestartButton();
  restart_restartbutton_id->set_component_source("restart.button");
  App.register_component(restart_restartbutton_id);
  App.register_button(restart_restartbutton_id);
  restart_restartbutton_id->set_name("Restart");
  restart_restartbutton_id->set_object_id("restart");
  restart_restartbutton_id->set_disabled_by_default(false);
  restart_restartbutton_id->set_icon("mdi:restart");
  restart_restartbutton_id->set_entity_category(::ENTITY_CATEGORY_DIAGNOSTIC);
  restart_restartbutton_id->set_device_class("restart");
  // button.safe_mode:
  //   platform: safe_mode
  //   name: Safe Mode Boot
  //   entity_category: diagnostic
  //   disabled_by_default: true
  //   id: safe_mode_safemodebutton_id
  //   icon: mdi:restart-alert
  //   device_class: restart
  //   safe_mode: safe_mode_safemodecomponent_id
  safe_mode_safemodebutton_id = new safe_mode::SafeModeButton();
  App.register_button(safe_mode_safemodebutton_id);
  safe_mode_safemodebutton_id->set_name("Safe Mode Boot");
  safe_mode_safemodebutton_id->set_object_id("safe_mode_boot");
  safe_mode_safemodebutton_id->set_disabled_by_default(true);
  safe_mode_safemodebutton_id->set_icon("mdi:restart-alert");
  safe_mode_safemodebutton_id->set_entity_category(::ENTITY_CATEGORY_DIAGNOSTIC);
  safe_mode_safemodebutton_id->set_device_class("restart");
  safe_mode_safemodebutton_id->set_component_source("safe_mode.button");
  App.register_component(safe_mode_safemodebutton_id);
  safe_mode_safemodebutton_id->set_safe_mode(safe_mode_safemodecomponent_id);
  // socket:
  //   implementation: bsd_sockets
  // md5:
  // network:
  //   enable_ipv6: false
  //   min_ipv6_addr_count: 0
  // =========== AUTO GENERATED CODE END ============
  App.setup();
}

void loop() {
  App.loop();
}
