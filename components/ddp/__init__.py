import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.light.types import AddressableLightEffect
from esphome.const import CONF_ID, CONF_NAME

DEPENDENCIES = ["network"]

ddp_ns = cg.esphome_ns.namespace("ddp")
DDPComponent = ddp_ns.class_("DDPComponent", cg.Component)
DDPLightEffectBase = ddp_ns.class_("DDPLightEffectBase", AddressableLightEffect)

CONF_DDP_ID = "ddp_id"
CONF_DDP_TIMEOUT = "timeout"
CONF_DDP_DIS_GAMMA = "disable_gamma"
CONF_DDP_SCALING = "brightness_scaling"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(DDPComponent),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
