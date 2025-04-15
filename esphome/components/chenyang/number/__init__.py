import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    CONF_DEVICE_CLASS,
    CONF_ID,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_SPEED,
    CONF_STEP,
    CONF_TYPE,
    DEVICE_CLASS_PRESSURE,
    DEVICE_CLASS_RESTART,
    DEVICE_CLASS_SPEED,
    ENTITY_CATEGORY_CONFIG,
)

from ..cover import chenyang_ns, ChenyangCover, CONF_CHENYANG_ID

DEPENDENCIES = ["cover.chenyang"]

ChenyangNumber = chenyang_ns.class_(
    "ChenyangNumber",
    number.Number,
    cg.Component,
    cg.Parented.template(ChenyangCover),
)

SpeedNumber = chenyang_ns.class_("SpeedNumber", ChenyangNumber)
TorqueNumber = chenyang_ns.class_("TorqueNumber", ChenyangNumber)

CONF_TORQUE = "torque"

_NUMBER_SCHEMA = (
    number.number_schema(
        ChenyangNumber,
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
        CONF_SPEED: _NUMBER_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(SpeedNumber),
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_SPEED): number.validate_device_class,
            }
        ),
        CONF_TORQUE: _NUMBER_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(TorqueNumber),
                cv.Optional(CONF_DEVICE_CLASS, default=DEVICE_CLASS_PRESSURE): number.validate_device_class,
            }
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_CHENYANG_ID])
    if config[CONF_TYPE] == CONF_SPEED:
        var = await number.new_number(config, min_value=1, max_value=3, step=1)
    if config[CONF_TYPE] == CONF_TORQUE:
        var = await number.new_number(config, min_value=0, max_value=5, step=1)
    await cg.register_component(var, config)
    await cg.register_parented(var, parent)
    cg.add(getattr(parent, f"set_{config[CONF_TYPE]}_number")(var))