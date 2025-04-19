import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_TYPE, ENTITY_CATEGORY_CONFIG

from ..cover import gm40_ns, GM40Cover, CONF_GM40_ID

DEPENDENCIES = ["cover.gm40"]

GM40Switch = gm40_ns.class_(
    "GM40Switch",
    switch.Switch,
    cg.Component,
    cg.Parented.template(GM40Cover),
)

InvertDirectionSwitch = gm40_ns.class_("InvertDirectionSwitch", GM40Switch)
LedIndicatorSwitch = gm40_ns.class_("LedIndicatorSwitch", GM40Switch)

CONF_INVERT_DIRECTION = "invert_direction"
CONF_LED_INDICATOR = "led_indicator"

ICON_LED_ON = "mdi:led-on"
ICON_SWAP_HORIZONTAL = "mdi:swap-horizontal"

_SWITCH_SCHEMA = (
    switch.switch_schema(
        GM40Switch,
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
        CONF_INVERT_DIRECTION: _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(InvertDirectionSwitch),
                cv.Optional(CONF_ICON, default=ICON_SWAP_HORIZONTAL): cv.icon,
            }
        ),
        CONF_LED_INDICATOR: _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(LedIndicatorSwitch),
                cv.Optional(CONF_ICON, default=ICON_LED_ON): cv.icon,
            }
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_GM40_ID])
    var = await switch.new_switch(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, parent)
    cg.add(getattr(parent, f"set_{config[CONF_TYPE]}_switch")(var))
