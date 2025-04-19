#include "chenyang_button.h"

namespace esphome {
namespace chenyang {

static const char *const TAG = "chenyang.button";

void GetStatusButton::press_action() {
  ESP_LOGI(TAG, "Getting status");
  uint8_t data[2] = {GET_STATUS, 0x00};
  this->parent_->send_command(data, 2);
}

void FactoryResetButton::press_action() {
  ESP_LOGI(TAG, "Sending factory reset command");
  uint8_t data[2] = {FACTORY_RESET, 0x00};
  this->parent_->send_command(data, 2);
}

void RestartButton::press_action() {
  ESP_LOGI(TAG, "Sending restart command");
  uint8_t data[2] = {RESTART, 0x00};
  this->parent_->send_command(data, 2);
}

}  // namespace chenyang
}  // namespace esphome
