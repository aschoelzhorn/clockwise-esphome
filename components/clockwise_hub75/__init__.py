# SPDX-FileCopyrightText: 2025 Clockwise Integration
# SPDX-License-Identifier: MIT

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import time

# No explicit DEPENDENCIES needed - the hub75_id schema reference ensures ordering.
# We pass HUB75Display pointer directly via codegen (no dynamic_cast/RTTI required).
CODEOWNERS = ["@clockwise"]

clockwise_hub75_ns = cg.esphome_ns.namespace("clockwise_hub75")
hub75_ns = cg.esphome_ns.namespace("hub75_display")
HUB75Display = hub75_ns.class_("HUB75Display", cg.PollingComponent)  # matches upstream declaration
ClockwiseHUB75 = clockwise_hub75_ns.class_("ClockwiseHUB75", cg.PollingComponent)

# Declare the C++ enum
ClockfaceType = clockwise_hub75_ns.enum("ClockfaceType")

CONF_CLOCKWISE_HUB75_ID = "clockwise_hub75_id"

CONF_CLOCKFACE_TYPE = "clockface_type"
CONF_INITIAL_BRIGHTNESS = "initial_brightness"
CONF_TIME_ID = "time_id"

CLOCKFACE_TYPES = {
    "PACMAN": ClockfaceType.PACMAN,
    "MARIO": ClockfaceType.MARIO,
    "CLOCK": ClockfaceType.CLOCK,
}

CONF_HUB75_ID = "hub75_id"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ClockwiseHUB75),
    cv.Required(CONF_HUB75_ID): cv.use_id(HUB75Display),
    cv.Optional(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
    cv.Optional(CONF_CLOCKFACE_TYPE, default="PACMAN"): cv.enum(CLOCKFACE_TYPES, upper=True),
    cv.Optional(CONF_INITIAL_BRIGHTNESS, default=128): cv.int_range(min=0, max=255),
}).extend(cv.polling_component_schema("16ms"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    # Get HUB75 display directly (explicit type, no dynamic_cast needed)
    hub75_var = await cg.get_variable(config[CONF_HUB75_ID])
    cg.add(var.set_hub75_display(hub75_var))
    
    # Get time component if provided
    if CONF_TIME_ID in config:
        time_var = await cg.get_variable(config[CONF_TIME_ID])
        cg.add(var.set_time(time_var))
    
    # Configure initial settings
    cg.add(var.set_clockface_type(config[CONF_CLOCKFACE_TYPE]))
    cg.add(var.set_initial_brightness(config[CONF_INITIAL_BRIGHTNESS]))