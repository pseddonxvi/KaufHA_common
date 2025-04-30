"""Addressable DDP light effect for ESPHome."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_NAME, CONF_ID, CONF_TIMEOUT

from . import ddp_ns, DDPComponent, CONF_DDP_ID

DEPENDENCIES = ["ddp"]

# Define effect-specific configuration options
CONF_DISABLE_GAMMA = "disable_gamma"
CONF_BRIGHTNESS_SCALING = "brightness_scaling"
CONF_OFFSET = "offset"

# Define the effect class
DDPAddressableLightEffect = ddp_ns.class_("DDPAddressableLightEffect", light.AddressableLightEffect)

# Define the brightness scaling options
BRIGHTNESS_SCALING = {
    "alpha": 0,
    "none": 1,
    "luminance": 2,
}

# Register with light effects
light.EFFECTS.register("addressable_ddp", {
    cv.GenerateID(): cv.declare_id(DDPAddressableLightEffect),
    cv.GenerateID(CONF_DDP_ID): cv.use_id(DDPComponent),
    cv.Optional(CONF_TIMEOUT, default="1s"): cv.positive_time_period_milliseconds,
    cv.Optional(CONF_DISABLE_GAMMA, default=False): cv.boolean,
    cv.Optional(CONF_BRIGHTNESS_SCALING, default="alpha"): cv.enum(BRIGHTNESS_SCALING, lower=True),
    cv.Optional(CONF_OFFSET, default=0): cv.positive_int,
}, validate_addressable=True)

async def setup_addressable_ddp(config, effect_id, template_args, args):
    """Set up an addressable DDP effect."""
    effect = cg.new_Pvariable(effect_id, config[CONF_NAME])
    parent = await cg.get_variable(config[CONF_DDP_ID])
    
    cg.add(effect.set_timeout(config[CONF_TIMEOUT]))
    cg.add(effect.set_disable_gamma(config[CONF_DISABLE_GAMMA]))
    cg.add(effect.set_brightness_scaling(config[CONF_BRIGHTNESS_SCALING]))
    cg.add(effect.set_offset(config[CONF_OFFSET]))
    cg.add(parent.add_effect(effect))
    
    return effect