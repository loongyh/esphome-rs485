#include "chenyang_switch.h"

namespace esphome {
namespace chenyang {

static const char *const TAG = "chenyang.switch";

void LedIndicatorSwitch::write_state(bool state) {
    ESP_LOGI(TAG, "%s LED indicator", state ? "Enabling" : "Disabling");
    uint8_t payload = state ? 0x01 : 0x00;
    uint8_t data[2] = {LED_INDICATOR, payload};
    this->parent_->send_command(data, 2);
}

void InvertDirectionSwitch::write_state(bool state) {
    ESP_LOGI(TAG, "%s invert direction", state ? "Enabling" : "Disabling");
    uint8_t payload = state ? 0x01 : 0x00;
    uint8_t data[2] = {INVERT_DIRECTION, payload};
    this->parent_->send_command(data, 2);
}

void PullToStartSwitch::write_state(bool state) {
    ESP_LOGI(TAG, "%s pull to start", state ? "Enabling" : "Disabling");
    uint8_t payload = state ? 0x00 : 0x01;
    uint8_t data[2] = {PULL_TO_START, payload};
    this->parent_->send_command(data, 2);
}

void CloseOnPowerOnSwitch::write_state(bool state) {
    ESP_LOGI(TAG, "%s close on power on", state ? "Enabling" : "Disabling");
    uint8_t payload = state ? 0x01 : 0x00;
    uint8_t data[2] = {CLOSE_ON_POWER_ON, payload};
    this->parent_->send_command(data, 2);
}

void OpenOnPowerOnSwitch::write_state(bool state) {
    ESP_LOGI(TAG, "%s open on power on", state ? "Enabling" : "Disabling");
    uint8_t payload = state ? 0x01 : 0x00;
    uint8_t data[2] = {OPEN_ON_POWER_ON, payload};
    this->parent_->send_command(data, 2);
}

void RainSensorSwitch::write_state(bool state) {
    ESP_LOGI(TAG, "%s rain sensor", state ? "Enabling" : "Disabling");
    uint8_t payload = state ? 0x00 : 0x01;
    uint8_t data[2] = {RAIN_SENSOR, static_cast<uint8_t>(state ? 0x00 : 0x01)};
    this->parent_->send_command(data, 2);
}

void RainInvertDirectionSwitch::write_state(bool state) {
    ESP_LOGI(TAG, "%s rain invert direction", state ? "Enabling" : "Disabling");
    uint8_t payload = state ? 0x01 : 0x00;
    uint8_t data[2] = {RAIN_INVERT_DIRECTION, payload};
    this->parent_->send_command(data, 2);
}

void LockSwitch::write_state(bool state) {
    ESP_LOGI(TAG, "Sending %s command", state ? "lock" : "unlock");
    uint8_t payload = state ? 0x01 : 0x00;
    uint8_t data[2] = {LOCK, payload};
    this->parent_->send_command(data, 2);
}

void PowerOffUnlockSwitch::write_state(bool state) {
    ESP_LOGI(TAG, "%s power off unlock", state ? "Enabling" : "Disabling");
    uint8_t payload = state ? 0x01 : 0x00;
    uint8_t data[2] = {POWER_OFF_UNLOCK, payload};
    this->parent_->send_command(data, 2);
}

}  // namespace chenyang
}  // namespace esphome
