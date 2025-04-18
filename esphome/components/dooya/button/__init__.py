import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import (
    CONF_DEVICE_CLASS,
    CONF_FACTORY_RESET,
    CONF_ICON,
    CONF_ID,
    CONF_TYPE,
    DEVICE_CLASS_RESTART,
    ENTITY_CATEGORY_CONFIG,
)

from ..cover import dooya_ns, DooyaCover, CONF_DOOYA_ID

DEPENDENCIES = ["cover.dooya"]

DooyaButton = dooya_ns.class_(
    "DooyaButton",
    button.Button,
    cg.Component,
    cg.Parented.template(DooyaCover),
)

GetStatusButton = dooya_ns.class_("GetStatusButton", DooyaButton)
ClearPositioningButton = dooya_ns.class_("ClearPositioningButton", DooyaButton)
FactoryResetButton = dooya_ns.class_("FactoryResetButton", DooyaButton)

CONF_CLEAR_POSITIONING = "clear_positioning"
CONF_GET_STATUS = "get_status"

ICON_NUKE = "mdi:nuke"
ICON_SYNC = "mdi:sync"

_BUTTON_SCHEMA = (
    button.button_schema(
        DooyaButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
    )
    .extend(
        {
            cv.GenerateID(CONF_DOOYA_ID): cv.use_id(DooyaCover),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

CONFIG_SCHEMA = cv.typed_schema(
    {
        CONF_GET_STATUS: _BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(GetStatusButton),
                cv.Optional(CONF_ICON, default=ICON_SYNC): cv.icon,
            }
        ),
        CONF_CLEAR_POSITIONING: _BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(ClearPositioningButton),
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_RESTART): button.validate_device_class,
            }
        ),
        CONF_FACTORY_RESET: _BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(FactoryResetButton),
                cv.Optional(CONF_ICON, default=ICON_NUKE): cv.icon,
            }
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_DOOYA_ID])
    var = await button.new_button(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, parent)
    cg.add(getattr(parent, f"set_{config[CONF_TYPE]}_button")(var))
