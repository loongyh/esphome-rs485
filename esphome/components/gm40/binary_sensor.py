import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC
from .cover import CONF_GM40_ID, GM40

CONF_UNKNOWN_POSITION = "unknown_position"
ICON_PROGRESS_QUESTION = "mdi:progress-question"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_GM40_ID): cv.use_id(GM40),
    cv.Required(CONF_UNKNOWN_POSITION): binary_sensor.binary_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon=ICON_PROGRESS_QUESTION,
    ),
}


async def to_code(config):
    gm40 = await cg.get_variable(config[CONF_GM40_ID])

    if CONF_UNKNOWN_POSITION in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_UNKNOWN_POSITION])
        cg.add(gm40.set_unknown_position_binary_sensor(sens))
