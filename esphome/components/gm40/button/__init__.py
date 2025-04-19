import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import (
    CONF_DEVICE_CLASS,
    CONF_FACTORY_RESET,
    CONF_ICON,
    CONF_ID,
    CONF_RESTART,
    CONF_TYPE,
    DEVICE_CLASS_RESTART,
    ENTITY_CATEGORY_CONFIG,
)

from ..cover import gm40_ns, GM40Cover, CONF_GM40_ID

DEPENDENCIES = ["cover.gm40"]

GM40Button = gm40_ns.class_(
    "GM40Button",
    button.Button,
    cg.Component,
    cg.Parented.template(GM40Cover),
)

GetStatusButton = gm40_ns.class_("GetStatusButton", GM40Button)
FactoryResetButton = gm40_ns.class_("FactoryResetButton", GM40Button)
RestartButton = gm40_ns.class_("RestartButton", GM40Button)

CONF_GET_STATUS = "get_status"

ICON_NUKE = "mdi:nuke"
ICON_SYNC = "mdi:sync"

_BUTTON_SCHEMA = (
    button.button_schema(
        GM40Button,
        entity_category=ENTITY_CATEGORY_CONFIG,
    )
    .extend(
        {
            cv.GenerateID(CONF_GM40_ID): cv.use_id(GM40Cover),
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
        CONF_FACTORY_RESET: _BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(FactoryResetButton),
                cv.Optional(CONF_ICON, default=ICON_NUKE): cv.icon,
            }
        ),
        CONF_RESTART: _BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(RestartButton),
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_RESTART): button.validate_device_class,
            }
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_GM40_ID])
    var = await button.new_button(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, parent)
    cg.add(getattr(parent, f"set_{config[CONF_TYPE]}_button")(var))
