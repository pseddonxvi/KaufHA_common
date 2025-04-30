"""DDP component for ESPHome. Modified for Raspberry Pi Pico W."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

# Import the component configuration
from .component import register_ddp_component, DDPNS  # noqa

# Export the namespace and component types
DDP_COMPONENT_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DDPNS.DDPComponent),
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    """Generate code for the DDP component."""
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
