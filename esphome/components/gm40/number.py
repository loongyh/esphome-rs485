import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import CONF_SPEED, DEVICE_CLASS_SPEED, ENTITY_CATEGORY_CONFIG

from .cover import gm40_ns, CONF_GM40_ID, GM40Cover

DEPENDENCIES = ["cover.gm40"]

GM40Number = gm40_ns.class_(
    "GM40Number",
    number.Number,
    cg.Component,
    cg.Parented.template(GM40Cover),
)

CONFIG_SCHEMA = number.number_schema(
    GM40Number,
    device_class=DEVICE_CLASS_SPEED,
    entity_category=ENTITY_CATEGORY_CONFIG,
).extend(
    {
        cv.GenerateID(CONF_GM40_ID): cv.use_id(GM40Cover),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_GM40_ID])
    var = await number.new_number(config, min_value=1, max_value=3, step=1)

    cg.add(parent.set_speed_number(var))
