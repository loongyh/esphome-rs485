#include "gm40_switch.h"

namespace esphome {
namespace gm40 {

static const char *const TAG = "gm40.switch";

void InvertDirectionSwitch::write_state(bool state) {
  ESP_LOGI(TAG, "%s invert direction", state ? "Enabling" : "Disabling");
  uint8_t payload = state ? 0x01 : 0x00;
  uint8_t data[5] = {CONTROL, 0x00, INVERT_DIRECTION, 0x00, payload};
  this->parent_->send_command(data, 5);
}

void LedIndicatorSwitch::write_state(bool state) {
  ESP_LOGI(TAG, "%s LED indicator", state ? "Enabling" : "Disabling");
  uint8_t payload = state ? 0x01 : 0x00;
  uint8_t data[5] = {CONTROL, 0x00, LED_INDICATOR, 0x00, payload};
  this->parent_->send_command(data, 5);
}

}  // namespace gm40
}  // namespace esphome
