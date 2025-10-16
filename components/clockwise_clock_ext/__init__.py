import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import time as time_ns
from esphome.components import binary_sensor
from esphome.components import select


CONF_PINS = "pins"
CONF_WIDTH="width"; CONF_HEIGHT="height"
CONF_BRIGHTNESS="brightness"; CONF_DRIVER="driver"; CONF_CLK_PHASE="clock_phase"; CONF_I2C_SPEED="i2c_speed"

CONF_R1="r1"; CONF_G1="g1"; CONF_B1="b1"
CONF_R2="r2"; CONF_G2="g2"; CONF_B2="b2"
CONF_A="a"; CONF_B="b"; CONF_C="c"; CONF_D="d"; CONF_E="e"
CONF_LAT="lat"; CONF_OE="oe"; CONF_CLK="clk"

CONF_BUTTON_ID = "button_id"

CONF_CLOCKFACE = "clockface"

ns = cg.esphome_ns.namespace("clockwise_clock_ext")
ClockwiseClk = ns.class_("ClockwiseClock", cg.PollingComponent)

CLOCKFACE_OPTIONS = ["pacman", "mario"]
clockface_map = {"pacman": 0, "mario": 1}

CONFIG_SCHEMA = cv.Schema({
  cv.GenerateID(): cv.declare_id(ClockwiseClk),

  cv.Required(CONF_WIDTH): cv.int_range(min=8),
  cv.Required(CONF_HEIGHT): cv.int_range(min=8),
  cv.Optional(CONF_BRIGHTNESS, default=180): cv.int_range(min=0, max=255),
  cv.Optional(CONF_DRIVER, default="FM6126A"): cv.one_of("SHIFTREG","FM6124","FM6126A","ICN2038S","MBI5124","DP3246"),
  cv.Optional(CONF_CLK_PHASE, default=True): cv.boolean,
  cv.Optional(CONF_I2C_SPEED, default='8M'): cv.one_of("8M", "10M","15M","16M","20M"),
  cv.Required(CONF_PINS): cv.Schema({
    cv.Required(CONF_R1): cv.int_, cv.Required(CONF_G1): cv.int_, cv.Required(CONF_B1): cv.int_,
    cv.Required(CONF_R2): cv.int_, cv.Required(CONF_G2): cv.int_, cv.Required(CONF_B2): cv.int_,
    cv.Required(CONF_A): cv.int_,  cv.Required(CONF_B): cv.int_,  cv.Required(CONF_C): cv.int_,
    cv.Required(CONF_D): cv.int_,  cv.Required(CONF_E): cv.int_,
    cv.Required(CONF_LAT): cv.int_, cv.Required(CONF_OE): cv.int_, cv.Required(CONF_CLK): cv.int_,
  }),
  cv.Optional(CONF_BUTTON_ID): cv.use_id(binary_sensor.BinarySensor),
  cv.Optional(CONF_CLOCKFACE, default="pacman"): cv.one_of(*CLOCKFACE_OPTIONS, lower=True),
  cv.Required("time_id"): cv.use_id(time_ns.RealTimeClock),
}).extend(cv.polling_component_schema("16ms"))

def to_code(config):
    var = cg.new_Pvariable(config["id"])
    yield cg.register_component(var, config)

    cg.add(var.set_dimensions(config[CONF_WIDTH], config[CONF_HEIGHT]))
    cg.add(var.set_brightness(config[CONF_BRIGHTNESS]))
    cg.add(var.set_driver(config[CONF_DRIVER]))
    cg.add(var.set_clkphase(config[CONF_CLK_PHASE]))
    cg.add(var.set_i2c_speed(config[CONF_I2C_SPEED]))

    # Set initial clockface
    cg.add(var.set_clockface_type(clockface_map[config[CONF_CLOCKFACE]]))

    p = config[CONF_PINS]
    for k in [CONF_R1,CONF_G1,CONF_B1,CONF_R2,CONF_G2,CONF_B2,CONF_A,CONF_B,CONF_C,CONF_D,CONF_E,CONF_LAT,CONF_OE,CONF_CLK]:
        cg.add(getattr(var, f"set_{k}_pin")(p[k]))
    
    # Set time        
    t = yield cg.get_variable(config["time_id"])
    cg.add(var.set_time(t))

    if CONF_BUTTON_ID in config:
        button = yield cg.get_variable(config[CONF_BUTTON_ID])
        cg.add(var.set_button_sensor(button))
