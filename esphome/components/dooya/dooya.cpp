#include "dooya.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace dooya {

static const char *const TAG = "dooya.cover";

using namespace esphome::cover;

bool validate_crc(const std::vector<uint8_t> &frame) {
  size_t len = frame.size();
  return crc16(&frame[0], len - 2) == ((uint16_t) frame[len - 2] << 8 | frame[len - 1]);
}

CoverTraits DooyaCover::get_traits() {
  auto traits = CoverTraits();
  traits.set_supports_stop(true);
  traits.set_supports_position(true);
  return traits;
}

void DooyaCover::control(const CoverCall &call) {
  if (call.get_stop()) {
    uint8_t data[2] = {CONTROL, STOP};
    this->send_command_(data, 2);
  } else if (call.get_position().has_value()) {
    auto pos = *call.get_position();
    if (pos != this->position) {
      if (pos == COVER_OPEN) {
        uint8_t data[2] = {CONTROL, OPEN};
        this->send_command_(data, 2);
      } else if (pos == COVER_CLOSED) {
        uint8_t data[2] = {CONTROL, CLOSE};
        this->send_command_(data, 2);
      } else {
        uint8_t data[3] = {CONTROL, SET_POSITION, (uint8_t) (pos * 100)};
        this->send_command_(data, 3);
      }
    }
  }
}

void DooyaCover::send_update() {
  if (this->current_operation != COVER_OPERATION_IDLE) {
    uint8_t data[3] = {READ, this->current_request_, 0x01};
    this->send_command_(data, 3);
  }
}

void DooyaCover::on_uart_multi_byte(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  switch (at) {
    case 0:
      if (byte == START_CODE)
        this->rx_buffer_.push_back(byte);
      break;
    case 1:
      if (byte == this->address_[0])
        this->rx_buffer_.push_back(byte);
      else
        this->rx_buffer_.clear();
      break;
    case 2:
      if (byte == this->address_[1])
        this->rx_buffer_.push_back(byte);
      else
        this->rx_buffer_.clear();
      break;
    case 3:
      if (byte == CONTROL || byte == READ)
        this->rx_buffer_.push_back(byte);
      else
        this->rx_buffer_.clear();
      break;
    case 6:
      this->rx_buffer_.push_back(byte);
      if (this->rx_buffer_[3] == CONTROL && this->rx_buffer_[4] != SET_POSITION) {
        if (validate_crc(this->rx_buffer_))
          this->process_response_();
        else
          ESP_LOGE(TAG, "Incoming data CRC check failed");
        this->rx_buffer_.clear();
      }
      break;
    case 7:
      this->rx_buffer_.push_back(byte);
      if (validate_crc(this->rx_buffer_)) {
        if (this->rx_buffer_[3] == CONTROL)
          this->process_response_();
        else
          this->process_status_();
      } else {
        ESP_LOGE(TAG, "Incoming data CRC check failed");
      }
      this->rx_buffer_.clear();
      break;
    default:
      this->rx_buffer_.push_back(byte);
  }
}

void DooyaCover::process_response_() {
  this->parent_->ready_to_tx = true;
  switch (this->rx_buffer_[4]) {
    case STOP:
      this->current_operation = COVER_OPERATION_IDLE;
      break;
    case OPEN:
      this->current_operation = COVER_OPERATION_OPENING;
      break;
    case CLOSE:
      this->current_operation = COVER_OPERATION_CLOSING;
      break;
    case SET_POSITION:
      if (this->rx_buffer_[5] > (uint8_t)(this->position * 100))
        this->current_operation = COVER_OPERATION_OPENING;
      else
        this->current_operation = COVER_OPERATION_CLOSING;
      break;
    default:
      ESP_LOGE(TAG, "Invalid control operation received");
      return;
  }
  this->publish_state(false);

}

void DooyaCover::process_status_() {
  this->parent_->ready_to_tx = true;
  if (this->current_request_ == GET_POSITION) {
    float pos = 0.5f;
    if (this->rx_buffer_[5] != 0xFF)
      pos = clamp((float) this->rx_buffer_[5] / 100, 0.0f, 1.0f);
    if (this->position != pos) {
      this->position = pos;
      this->publish_state(false);
    }
    this->current_request_ = GET_STATUS;
  } else {
    switch (this->rx_buffer_[5]) {
      case 0:
        if (this->current_operation != COVER_OPERATION_IDLE) {
          this->current_operation = COVER_OPERATION_IDLE;
          this->publish_state(false);
        }
        break;
      case 1:
        if (this->current_operation != COVER_OPERATION_OPENING) {
          this->current_operation = COVER_OPERATION_OPENING;
          this->publish_state(false);
        }
        break;
      case 2:
        if (this->current_operation != COVER_OPERATION_CLOSING) {
          this->current_operation = COVER_OPERATION_CLOSING;
          this->publish_state(false);
        }
        break;
      default:
        ESP_LOGE(TAG, "Invalid status operation received");
        return;
    }
    this->current_request_ = GET_POSITION;
  }
}

void DooyaCover::send_command_(const uint8_t *data, uint8_t len) {
  std::vector<uint8_t> frame = {START_CODE, this->address_[0], this->address_[1]};
  for (size_t i = 0; i < len; i++) {
    frame.push_back(data[i]);
  }
  uint16_t crc = crc16(&frame[0], frame.size());
  frame.push_back(crc >> 0);
  frame.push_back(crc >> 8);

  this->send(frame);
}

void DooyaCover::dump_config() {
  ESP_LOGCONFIG(TAG, "Dooya:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X%02X", this->address_[0], this->address_[1]);
}

}  // namespace dooya
}  // namespace esphome
