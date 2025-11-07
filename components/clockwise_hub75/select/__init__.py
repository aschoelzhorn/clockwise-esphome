# SPDX-FileCopyrightText: 2025 Clockwise Integration
# SPDX-License-Identifier: MIT

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import (
    CONF_ID,
    CONF_ENTITY_CATEGORY,
    ENTITY_CATEGORY_CONFIG,
)

from .. import clockwise_hub75_ns, ClockwiseHUB75, CONF_CLOCKWISE_HUB75_ID

DEPENDENCIES = ["clockwise_hub75"]

ClockwiseSelect = clockwise_hub75_ns.class_("ClockwiseSelect", select.Select, cg.Component)

CONFIG_SCHEMA = select.select_schema(
    ClockwiseSelect,
    entity_category=ENTITY_CATEGORY_CONFIG,
).extend({
    cv.GenerateID(CONF_CLOCKWISE_HUB75_ID): cv.use_id(ClockwiseHUB75),
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    options = ["Pacman", "Mario", "Clock", "Dune"]
    var = await select.new_select(config, options=options)
    await cg.register_component(var, config)
    
    parent = await cg.get_variable(config[CONF_CLOCKWISE_HUB75_ID])
    cg.add(var.set_clockwise_hub75(parent))