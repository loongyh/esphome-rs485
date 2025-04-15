import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import (
    CONF_DEVICE_CLASS,
    CONF_ICON,
    CONF_ID,
    CONF_TYPE,
    DEVICE_CLASS_RESTART,
    ENTITY_CATEGORY_CONFIG
)

from ..cover import chenyang_ns, ChenyangCover, CONF_CHENYANG_ID

DEPENDENCIES = ["cover.chenyang"]

ChenyangButton = chenyang_ns.class_(
    "ChenyangButton",
    button.Button,
    cg.Component,
    cg.Parented.template(ChenyangCover),
)

GetStatusButton = chenyang_ns.class_("GetStatusButton", ChenyangButton)
FactoryResetButton = chenyang_ns.class_("FactoryResetButton", ChenyangButton)
RestartButton = chenyang_ns.class_("RestartButton", ChenyangButton)

ICON_NUKE = "mdi:nuke"
ICON_SYNC = "mdi:sync"

_BUTTON_SCHEMA = (
    button.button_schema(
        ChenyangButton,
        entity_category=ENTITY_CATEGORY_CONFIG,
    )
    .extend(
        {
            cv.GenerateID(CONF_CHENYANG_ID): cv.use_id(ChenyangCover),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)

CONFIG_SCHEMA = cv.typed_schema(
    {
        "get_status": _BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(GetStatusButton),
                cv.Optional(CONF_ICON, default=ICON_SYNC): cv.icon,
            }
        ),
        "factory_reset": _BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(FactoryResetButton),
                cv.Optional(CONF_ICON, default=ICON_NUKE): cv.icon,
            }
        ),
        "restart": _BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(RestartButton),
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_RESTART): button.validate_device_class,
            }
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_CHENYANG_ID])
    var = await button.new_button(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, parent)
    cg.add(getattr(parent, f"set_{config[CONF_TYPE]}_button")(var))