import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC
from .cover import CONF_CHENYANG_ID, Chenyang

CONF_UNKNOWN_POSITION = "unknown_position"
ICON_PROGRESS_QUESTION = "mdi:progress-question"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_CHENYANG_ID): cv.use_id(Chenyang),
    cv.Required(CONF_UNKNOWN_POSITION): binary_sensor.binary_sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon=ICON_PROGRESS_QUESTION,
    ),
}


async def to_code(config):
    chenyang = await cg.get_variable(config[CONF_CHENYANG_ID])

    if CONF_UNKNOWN_POSITION in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_UNKNOWN_POSITION])
        cg.add(chenyang.set_unknown_position_binary_sensor(sens))
