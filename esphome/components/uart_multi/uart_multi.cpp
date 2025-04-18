#include "uart_multi.h"
#include "esphome/core/log.h"

namespace esphome {
namespace uart_multi {

static const char *const TAG = "uart_multi";

void UARTMulti::loop() {
  uint8_t byte;
  while (this->available()) {
    this->read_byte(&byte);
    for (auto *device : this->devices_)
      device->on_uart_multi_byte(byte);
  }
  if (millis() - this->last_tx_ > 1000) {
    this->ready_to_tx = true;
  }
  if (this->ready_to_tx && !this->tx_buffer_.empty()) {
    this->write_array(this->tx_buffer_.front());
    this->tx_buffer_.pop();
    this->last_tx_ = millis();
    this->ready_to_tx = false;
  }
}

void UARTMulti::update() {
  if (this->ready_to_tx) {
    this->devices_[this->update_token_]->send_update();
    this->update_token_ = ++this->update_token_ % this->devices_.size();
  }
}

void UARTMulti::dump_config() {
  ESP_LOGCONFIG(TAG, "UART Multi:");
  ESP_LOGCONFIG(TAG, "%u configured devices", this->devices_.size());  // NOLINT
  ESP_LOGCONFIG(TAG, "tx queue size = %u", this->tx_queue_size_);
}
float UARTMulti::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}
void UARTMulti::send(const std::vector<uint8_t> &data) {
  if (this->tx_buffer_.size() < this->tx_queue_size_) {
    this->tx_buffer_.push(data);
  } else {
    ESP_LOGW(TAG, "Queue is full, dropping frame");
  }
}

}  // namespace uart_multi
}  // namespace esphome
