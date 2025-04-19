#include "gm40_button.h"

namespace esphome {
namespace gm40 {

static const char *const TAG = "gm40.button";

void GetStatusButton::press_action() {
  ESP_LOGI(TAG, "Getting status");
  for (uint8_t read_type : {POSITION, INVERT_DIRECTION, SPEED, LED_INDICATOR}) {
    uint8_t data[5] = {READ, 0x00, read_type, 0x00, 0x01};
    this->parent_->send_command(data, 5);
  }
}

void FactoryResetButton::press_action() {
  ESP_LOGI(TAG, "Sending factory reset command");
  uint8_t data[5] = {CONTROL, 0x00, FACTORY_RESET, 0x00, 0x01};
  this->parent_->send_command(data, 5);
}

void RestartButton::press_action() {
  ESP_LOGI(TAG, "Sending restart command");
  uint8_t data[5] = {CONTROL, 0x00, RESTART, 0x00, 0x01};
  this->parent_->send_command(data, 5);
}

}  // namespace gm40
}  // namespace esphome
