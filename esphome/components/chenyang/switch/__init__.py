import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_TYPE, ENTITY_CATEGORY_CONFIG

from ..cover import chenyang_ns, ChenyangCover, CONF_CHENYANG_ID

DEPENDENCIES = ["cover.chenyang"]

ChenyangSwitch = chenyang_ns.class_(
    "ChenyangSwitch",
    switch.Switch,
    cg.Component,
    cg.Parented.template(ChenyangCover),
)

LedIndicatorSwitch = chenyang_ns.class_("LedIndicatorSwitch", ChenyangSwitch)
InvertDirectionSwitch = chenyang_ns.class_("InvertDirectionSwitch", ChenyangSwitch)
PullToStartSwitch = chenyang_ns.class_("PullToStartSwitch", ChenyangSwitch)
CloseOnPowerOnSwitch = chenyang_ns.class_("CloseOnPowerOnSwitch", ChenyangSwitch)
OpenOnPowerOnSwitch = chenyang_ns.class_("OpenOnPowerOnSwitch", ChenyangSwitch)
RainSensorSwitch = chenyang_ns.class_("RainSensorSwitch", ChenyangSwitch)
RainInvertDirectionSwitch = chenyang_ns.class_("RainInvertDirectionSwitch", ChenyangSwitch)
LockSwitch = chenyang_ns.class_("LockSwitch", ChenyangSwitch)
PowerOffUnlockSwitch = chenyang_ns.class_("PowerOffUnlockSwitch", ChenyangSwitch)

ICON_CALENDAR_COLLAPSE_HORIZONTAL = "mdi:calendar-collapse-horizontal"
ICON_CALENDAR_EXPAND_HORIZONTAL = "mdi:calendar-expand-horizontal"
ICON_HAND_WAVE = "mdi:hand-wave"
ICON_LED_ON = "mdi:led-on"
ICON_LOCK = "mdi:lock"
ICON_LOCK_RESET = "mdi:lock-reset"
ICON_SWAP_HORIZONTAL = "mdi:swap-horizontal"
ICON_SWAP_HORIZONTAL_HIDDEN = "mdi:swap-horizontal-hidden"
ICON_WEATHER_POURING = "mdi:weather-pouring"

_SWITCH_SCHEMA = (
    switch.switch_schema(
        ChenyangSwitch,
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
        "led_indicator": _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(LedIndicatorSwitch),
                cv.Optional(CONF_ICON, default=ICON_LED_ON): cv.icon,
            }
        ),
        "invert_direction": _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(InvertDirectionSwitch),
                cv.Optional(CONF_ICON, default=ICON_SWAP_HORIZONTAL): cv.icon,
            }
        ),
        "pull_to_start": _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(PullToStartSwitch),
                cv.Optional(CONF_ICON, default=ICON_HAND_WAVE): cv.icon,
            }
        ),
        "close_on_power_on": _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(CloseOnPowerOnSwitch),
                cv.Optional(CONF_ICON, default=ICON_CALENDAR_COLLAPSE_HORIZONTAL): cv.icon,
            }
        ),
        "open_on_power_on": _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(OpenOnPowerOnSwitch),
                cv.Optional(CONF_ICON, default=ICON_CALENDAR_EXPAND_HORIZONTAL): cv.icon,
            }
        ),
        "rain_sensor": _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(RainSensorSwitch),
                cv.Optional(CONF_ICON, default=ICON_WEATHER_POURING): cv.icon,
            }
        ),
        "rain_invert_direction": _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(RainInvertDirectionSwitch),
                cv.Optional(CONF_ICON, default=ICON_SWAP_HORIZONTAL_HIDDEN): cv.icon,
            }
        ),
        "lock": _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(LockSwitch),
                cv.Optional(CONF_ICON, default=ICON_LOCK): cv.icon,
            }
        ),
        "power_off_unlock": _SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(PowerOffUnlockSwitch),
                cv.Optional(CONF_ICON, default=ICON_LOCK_RESET): cv.icon,
            }
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_CHENYANG_ID])
    var = await switch.new_switch(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, parent)
    cg.add(getattr(parent, f"set_{config[CONF_TYPE]}_switch")(var))
