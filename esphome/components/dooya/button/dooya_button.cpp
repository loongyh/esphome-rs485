#include "dooya_button.h"

namespace esphome {
namespace dooya {

static const char *const TAG = "dooya.button";

void GetStatusButton::press_action() {
    ESP_LOGI(TAG, "Getting status");
    uint8_t data[2] = {CONTROL, GET_STATUS};
    this->parent_->send_command(data, 2);
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
