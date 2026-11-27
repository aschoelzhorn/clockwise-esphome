# SPDX-FileCopyrightText: 2025 Clockwise Integration
# SPDX-License-Identifier: MIT

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_HOUR, CONF_ID, CONF_MINUTE

# No explicit DEPENDENCIES needed - the hub75_id schema reference ensures ordering.
# We pass HUB75Display pointer directly via codegen (no dynamic_cast/RTTI required).
CODEOWNERS = ["@clockwise"]

clockwise_hub75_ns = cg.esphome_ns.namespace("clockwise_hub75")
# Use MockObj to match hub75 component's fully-qualified namespace
hub75_ns = cg.MockObj("::esphome::hub75", "::")
HUB75Display = hub75_ns.class_("HUB75Display")
ClockwiseHUB75 = clockwise_hub75_ns.class_("ClockwiseHUB75", cg.PollingComponent)

# Declare the C++ enum
ClockfaceType = clockwise_hub75_ns.enum("ClockfaceType")
PanelColorOrder = clockwise_hub75_ns.enum("PanelColorOrder")

CONF_CLOCKWISE_HUB75_ID = "clockwise_hub75_id"

CONF_CLOCKFACE_TYPE = "clockface_type"
CONF_INITIAL_BRIGHTNESS = "initial_brightness"
CONF_PANEL_COLOR_ORDER = "panel_color_order"
CONF_SPECIAL_DATES = "special_dates"
CONF_NIGHT_MODE_START = "night_mode_start"
CONF_NIGHT_MODE_END = "night_mode_end"

NIGHT_MODE_TIME = cv.All(
    cv.string_strict,
    cv.Length(min=5, max=5),
    cv.time_of_day,
)

CLOCKFACE_TYPES = {
    "PACMAN": ClockfaceType.PACMAN,
    "MARIO": ClockfaceType.MARIO,
    "CLOCK": ClockfaceType.CLOCK,
}

PANEL_COLOR_ORDERS = {
    "RGB": PanelColorOrder.RGB,
    "RBG": PanelColorOrder.RBG,
    "GRB": PanelColorOrder.GRB,
    "GBR": PanelColorOrder.GBR,
    "BRG": PanelColorOrder.BRG,
    "BGR": PanelColorOrder.BGR,
}

CONF_HUB75_ID = "hub75_id"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(ClockwiseHUB75),
    cv.Required(CONF_HUB75_ID): cv.use_id(HUB75Display),
    cv.Optional(CONF_CLOCKFACE_TYPE, default="PACMAN"): cv.enum(CLOCKFACE_TYPES, upper=True),
    cv.Optional(CONF_PANEL_COLOR_ORDER, default="RGB"): cv.enum(PANEL_COLOR_ORDERS, upper=True),
    cv.Optional(CONF_SPECIAL_DATES, default=""): cv.string,
    cv.Optional(CONF_INITIAL_BRIGHTNESS, default=128): cv.int_range(min=0, max=255),
    cv.Optional(CONF_NIGHT_MODE_START, default="20:00"): NIGHT_MODE_TIME,
    cv.Optional(CONF_NIGHT_MODE_END, default="06:00"): NIGHT_MODE_TIME,
}).extend(cv.polling_component_schema("16ms"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    hub75_var = await cg.get_variable(config[CONF_HUB75_ID])
    cg.add(var.set_hub75_display(hub75_var))

    cg.add(var.set_clockface_type(config[CONF_CLOCKFACE_TYPE]))
    cg.add(var.set_panel_color_order(config[CONF_PANEL_COLOR_ORDER]))
    cg.add(var.set_special_dates(config[CONF_SPECIAL_DATES]))
    cg.add(var.set_initial_brightness(config[CONF_INITIAL_BRIGHTNESS]))
    start = config[CONF_NIGHT_MODE_START]
    end = config[CONF_NIGHT_MODE_END]
    cg.add(var.set_night_start(start[CONF_HOUR] * 3600 + start[CONF_MINUTE] * 60))
    cg.add(var.set_night_end(end[CONF_HOUR] * 3600 + end[CONF_MINUTE] * 60))