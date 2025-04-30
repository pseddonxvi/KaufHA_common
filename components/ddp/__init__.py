"""DDP component for ESPHome. Modified for Raspberry Pi Pico W."""

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@KaufHA"]
DEPENDENCIES = ["network"]
MULTI_CONF = True

# Define the namespace
CONF_DDP_ID = "ddp_id"
ddp_ns = cg.esphome_ns.namespace("ddp")
DDPComponent = ddp_ns.class_("DDPComponent", cg.Component)

# This is what ESPHome looks for to load components
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DDPComponent),
}).extend(cv.COMPONENT_SCHEMA)

# Register the component with ESPHome
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    # Make the DDP component a global variable
    cg.add_global(ddp_ns.namespace("ddp").add_define("global_ddp_component", cg.RawExpression("nullptr")))