#include "dooya_switch.h"

namespace esphome {
namespace dooya {

static const char *const TAG = "dooya.switch";

void InvertDirectionSwitch::write_state(bool state) {
    ESP_LOGI(TAG, "%s invert direction", state ? "Enabling" : "Disabling");
    uint8_t payload = state ? 0x01 : 0x00;
    uint8_t data[4] = {WRITE, INVERT_DIRECTION, 0x01, payload};
    this->parent_->send_command(data, 2);
    this->parent_->current_write_payload = payload;
}

void PullToStartSwitch::write_state(bool state) {
    ESP_LOGI(TAG, "%s pull to start", state ? "Enabling" : "Disabling");
    uint8_t payload = state ? 0x00 : 0x01;
    uint8_t data[4] = {WRITE, PULL_TO_START, 0x01, payload};
    this->parent_->send_command(data, 2);
    this->parent_->current_write_payload = payload;
}

}  // namespace dooya
}  // namespace esphome
