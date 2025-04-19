#include "chenyang_number.h"

namespace esphome {
namespace chenyang {

static const char *const TAG = "chenyang.number";

void SpeedNumber::control(float value) {
  uint8_t target = static_cast<uint8_t>(value);
  ESP_LOGI(TAG, "Setting speed to %u", target);
  uint8_t data[2] = {SPEED, target};
  this->parent_->send_command(data, 2);
}

void TorqueNumber::control(float value) {
  uint8_t target = static_cast<uint8_t>(value);
  ESP_LOGI(TAG, "Setting torque to %u", target);
  uint8_t data[2] = {TORQUE, target};
  this->parent_->send_command(data, 2);
}

}  // namespace chenyang
}  // namespace esphome
