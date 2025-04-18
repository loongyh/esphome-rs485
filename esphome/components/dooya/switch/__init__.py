import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_TYPE, ENTITY_CATEGORY_CONFIG

from ..cover import dooya_ns, DooyaCover, CONF_DOOYA_ID

DEPENDENCIES = ["cover.dooya"]

DooyaSwitch = dooya_ns.class_(
    "DooyaSwitch",
    switch.Switch,
    cg.Component,
    cg.Parented.template(DooyaCover),
)

InvertDirectionSwitch = dooya_ns.class_("InvertDirectionSwitch", DooyaSwitch)
PullToStartSwitch = dooya_ns.class_("PullToStartSwitch", DooyaSwitch)

CONF_INVERT_DIRECTION = "invert_direction"
CONF_PULL_TO_START = "pull_to_start"

ICON_HAND_WAVE = "mdi:hand-wave"
ICON_SWAP_HORIZONTAL = "mdi:swap-horizontal"


_SWITCH_SCHEMA = (
    switch.switch_schema(
        DooyaSwitch,
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
        CONF_INVERT_DIRECTION: _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(InvertDirectionSwitch),
                cv.Optional(CONF_ICON, default=ICON_SWAP_HORIZONTAL): cv.icon,
            }
        ),
        CONF_PULL_TO_START: _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(PullToStartSwitch),
                cv.Optional(CONF_ICON, default=ICON_HAND_WAVE): cv.icon,
            }
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_DOOYA_ID])
    var = await switch.new_switch(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, parent)
    cg.add(getattr(parent, f"set_{config[CONF_TYPE]}_switch")(var))
