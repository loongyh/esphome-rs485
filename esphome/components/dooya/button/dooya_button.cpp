#include "dooya_button.h"

namespace esphome {
namespace dooya {

static const char *const TAG = "dooya.button";

void GetStatusButton::press_action() {
  ESP_LOGI(TAG, "Getting status");
  for (uint8_t read_type : {GET_STATUS, GET_POSITION, INVERT_DIRECTION, PULL_TO_START}) {
    uint8_t data[3] = {READ, read_type, 0x01};
    this->parent_->send_command(data, 3);
    this->parent_->read_requests.push({read_type, millis()});
  }
}

void ClearPositioningButton::press_action() {
  ESP_LOGI(TAG, "Sending clear positioning command");
  uint8_t data[2] = {CONTROL, CLEAR_POSITIONING};
  this->parent_->send_command(data, 2);
}

void FactoryResetButton::press_action() {
  ESP_LOGI(TAG, "Sending factory reset command");
  uint8_t data[2] = {CONTROL, FACTORY_RESET};
  this->parent_->send_command(data, 2);
}

}  // namespace dooya
}  // namespace esphome
