"""DDP component for ESPHome. Modified for Raspberry Pi Pico W."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_NAME, CONF_TIMEOUT
from esphome.components import light

# Define namespace and components
CODEOWNERS = ["@KaufHA"]
DEPENDENCIES = ["network"]

CONF_DDP_ID = "ddp_id"
CONF_DISABLE_GAMMA = "disable_gamma"
CONF_BRIGHTNESS_SCALING = "brightness_scaling"
CONF_OFFSET = "offset"

ddp_ns = cg.esphome_ns.namespace("ddp")
DDPComponent = ddp_ns.class_("DDPComponent", cg.Component)
DDPAddressableLightEffect = ddp_ns.class_("DDPAddressableLightEffect", light.AddressableLightEffect)
DDPLightEffectBase = ddp_ns.class_("DDPLightEffectBase")

BRIGHTNESS_SCALING = {
    "alpha": 0,
    "none": 1,
    "luminance": 2,
}

# DDP Component schema
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DDPComponent)
}).extend(cv.COMPONENT_SCHEMA)

# Register the addressable light effect
ADDRESSABLE_LIGHT_EFFECT_SCHEMA = light.ADDRESSABLE_LIGHT_EFFECT_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(DDPAddressableLightEffect),
    cv.GenerateID(CONF_DDP_ID): cv.use_id(DDPComponent),
    cv.Optional(CONF_TIMEOUT, default="1s"): cv.positive_time_period_milliseconds,
    cv.Optional(CONF_DISABLE_GAMMA, default=False): cv.boolean,
    cv.Optional(CONF_BRIGHTNESS_SCALING, default="alpha"): cv.enum(BRIGHTNESS_SCALING, lower=True),
    cv.Optional(CONF_OFFSET, default=0): cv.positive_int,
})

# Register the effect with ESPHome
@light.register_addressable_effect("addressable_ddp", ADDRESSABLE_LIGHT_EFFECT_SCHEMA)
async def addressable_ddp_effect_to_code(config, effect_id):
    parent = await cg.get_variable(config[CONF_DDP_ID])
    var = cg.new_Pvariable(effect_id, config[CONF_NAME])
    await light.register_addressable_effect(var, config)
    cg.add(var.set_timeout(config[CONF_TIMEOUT]))
    cg.add(var.set_disable_gamma(config[CONF_DISABLE_GAMMA]))
    cg.add(var.set_brightness_scaling(config[CONF_BRIGHTNESS_SCALING]))
    cg.add(var.set_offset(config[CONF_OFFSET]))
    cg.add(parent.add_effect(var))
    return var

# Component registration code
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
