"""Component registration for DDP."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

# Define the namespace for our component
DDPNS = cg.esphome_ns.namespace("ddp")

# Create the DDP component type
DDPComponent = DDPNS.class_("DDPComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DDPComponent),
}).extend(cv.COMPONENT_SCHEMA)

# Register the component with ESPHome
def register_ddp_component(config):
    var = cg.new_Pvariable(config[CONF_ID])
    return cg.register_component(var, config)

# Register the component for use with ESPHome
async def to_code(config):
    """Generate code for the DDP component."""
    await register_ddp_component(config)
