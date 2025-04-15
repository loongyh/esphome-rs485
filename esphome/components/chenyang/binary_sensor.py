import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import DEVICE_CLASS_PROBLEM, ENTITY_CATEGORY_DIAGNOSTIC

from .cover import CONF_CHENYANG_ID, ChenyangCover

DEPENDENCIES = ["cover.chenyang"]

CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(
    device_class=DEVICE_CLASS_PROBLEM,
    entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
).extend(
    {
        cv.GenerateID(CONF_CHENYANG_ID): cv.use_id(ChenyangCover),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_CHENYANG_ID])
    var = await binary_sensor.new_binary_sensor(config)

    cg.add(parent.set_positioning_binary_sensor(var))
