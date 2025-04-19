#include "dooya.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace dooya {

static const char *const TAG = "dooya.cover";

using namespace esphome::cover;

bool validate_crc(const std::vector<uint8_t> &frame) {
  size_t len = frame.size();
  return crc16(&frame[0], len - 2) == ((uint16_t) frame[len - 1] << 8 | frame[len - 2]);
}

CoverTraits DooyaCover::get_traits() {
  auto traits = CoverTraits();
  traits.set_supports_stop(true);
  traits.set_supports_position(true);
  return traits;
}

void DooyaCover::setup() {
  for (uint8_t read_type : {GET_STATUS, GET_POSITION, INVERT_DIRECTION, PULL_TO_START}) {
    uint8_t data[3] = {READ, read_type, 0x01};
    this->send_command(data, 3);
    this->read_requests.push({read_type, millis()});
  }
}

void DooyaCover::loop() {
  if (!this->read_requests.empty() && millis() - std::get<1>(this->read_requests.front()) > 1000)
    this->read_requests.pop();
}

void DooyaCover::control(const CoverCall &call) {
  if (call.get_stop()) {
    uint8_t data[2] = {CONTROL, STOP};
    this->send_command(data, 2);
  } else if (call.get_position().has_value()) {
    auto pos = *call.get_position();
    if ((uint8_t) (pos * 100) != (uint8_t) (this->position * 100)) {
      if (pos == COVER_OPEN) {
        uint8_t data[2] = {CONTROL, OPEN};
        this->send_command(data, 2);
      } else if (pos == COVER_CLOSED) {
        uint8_t data[2] = {CONTROL, CLOSE};
        this->send_command(data, 2);
      } else {
        uint8_t data[3] = {CONTROL, SET_POSITION, (uint8_t) (pos * 100)};
        this->send_command(data, 3);
      }
    }
  }
}

void DooyaCover::send_update() {
  if (this->current_operation != COVER_OPERATION_IDLE) {
    for (uint8_t read_type : {GET_STATUS, GET_POSITION}) {
      uint8_t data[3] = {READ, read_type, 0x01};
      this->send_command(data, 3);
      this->read_requests.push({read_type, millis()});
    }
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
      if (byte == READ || byte == WRITE || byte == CONTROL)
        this->rx_buffer_.push_back(byte);
      else
        this->rx_buffer_.clear();
      break;
    case 6:
      this->rx_buffer_.push_back(byte);
      if (this->rx_buffer_[3] == CONTROL && this->rx_buffer_[4] != SET_POSITION) {
        if (validate_crc(this->rx_buffer_))
          this->process_control_response_();
        else
          ESP_LOGE(TAG, "Incoming data CRC check failed");
        this->rx_buffer_.clear();
        this->parent_->ready_to_tx = true;
      }
      break;
    case 7:
      this->rx_buffer_.push_back(byte);
      if (validate_crc(this->rx_buffer_)) {
        switch (this->rx_buffer_[3]) {
          case READ:
            if (!this->read_requests.empty())
              this->process_read_response_();
            break;
          case WRITE:
            this->process_write_response_();
            break;
          case CONTROL:
            this->process_control_response_();
            break;
          default:
            ESP_LOGE(TAG, "Invalid response type received");
            break;
        }
      } else {
        ESP_LOGE(TAG, "Incoming data CRC check failed");
      }
      this->rx_buffer_.clear();
      this->parent_->ready_to_tx = true;
      break;
    default:
      this->rx_buffer_.push_back(byte);
      break;
  }
}

void DooyaCover::process_read_response_() {
  switch (std::get<0>(this->read_requests.front())) {
    case GET_POSITION:
      if (this->rx_buffer_[5] != UNKNOWN_POSITION) {
        if ((uint8_t) (this->position * 100) != this->rx_buffer_[5]) {
          this->position = clamp((float) this->rx_buffer_[5] / 100, 0.0f, 1.0f);
          this->publish_state(false);
        }
      }
#ifdef USE_BINARY_SENSOR
      if (this->positioning_binary_sensor_ != nullptr)
        this->positioning_binary_sensor_->publish_state(this->rx_buffer_[5] == UNKNOWN_POSITION);
#endif
      break;
#ifdef USE_SWITCH
    case INVERT_DIRECTION:
      if (this->invert_direction_switch_ != nullptr)
        this->invert_direction_switch_->publish_state(this->rx_buffer_[5] == 0x01);
      break;
    case PULL_TO_START:
      if (this->pull_to_start_switch_ != nullptr)
        this->pull_to_start_switch_->publish_state(this->rx_buffer_[5] == 0x00);
      break;
#endif
    case GET_STATUS:
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
        case 3:
          ESP_LOGW(TAG, "Device is in setting mode");
          break;
        default:
          ESP_LOGE(TAG, "Invalid status operation received");
          break;
      }
      break;
    default:
      ESP_LOGE(TAG, "Invalid read response received");
      break;
  }
  this->read_requests.pop();
}

void DooyaCover::process_write_response_() {
  switch (this->rx_buffer_[4]) {
#ifdef USE_SWITCH
    case INVERT_DIRECTION:
      if (this->invert_direction_switch_ != nullptr)
        this->invert_direction_switch_->publish_state(this->current_write_payload == 0x01);
      break;
    case PULL_TO_START:
      if (this->pull_to_start_switch_ != nullptr)
        this->pull_to_start_switch_->publish_state(this->current_write_payload == 0x00);
      break;
#endif
    default:
      ESP_LOGE(TAG, "Invalid write response received");
      break;
  }
}

void DooyaCover::process_control_response_() {
  switch (this->rx_buffer_[4]) {
    case OPEN:
      this->current_operation = COVER_OPERATION_OPENING;
      this->publish_state(false);
      break;
    case CLOSE:
      this->current_operation = COVER_OPERATION_CLOSING;
      this->publish_state(false);
      break;
    case STOP:
      this->current_operation = COVER_OPERATION_IDLE;
      this->publish_state(false);
      break;
    case SET_POSITION:
      if (this->rx_buffer_[5] != UNKNOWN_POSITION) {
        if (this->rx_buffer_[5] > (uint8_t) (this->position * 100))
          this->current_operation = COVER_OPERATION_OPENING;
        else
          this->current_operation = COVER_OPERATION_CLOSING;
        this->publish_state(false);
      }
#ifdef USE_BINARY_SENSOR
      if (this->positioning_binary_sensor_ != nullptr)
        this->positioning_binary_sensor_->publish_state(this->rx_buffer_[5] == UNKNOWN_POSITION);
#endif
      break;
    case CLEAR_POSITIONING:
      ESP_LOGI(TAG, "Positioning cleared");
      break;
    case FACTORY_RESET:
      ESP_LOGI(TAG, "Factory reset successful");
      break;
    default:
      ESP_LOGE(TAG, "Invalid control response received");
      break;
  }
}

void DooyaCover::send_command(const uint8_t *data, uint8_t len) {
  std::vector<uint8_t> frame = {START_CODE, this->address_[0], this->address_[1]};
  frame.insert(frame.end(), data, data + len);
  uint16_t crc = crc16(&frame[0], frame.size());
  frame.push_back(crc >> 0);
  frame.push_back(crc >> 8);

  this->send(frame);
}

void DooyaCover::dump_config() {
  ESP_LOGCONFIG(TAG, "Dooya:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X%02X", this->address_[0], this->address_[1]);
#ifdef USE_BINARY_SENSOR
  LOG_BINARY_SENSOR("  ", "Positioning", this->positioning_binary_sensor_);
#endif
#ifdef USE_BUTTON
  LOG_BUTTON("  ", "Get Status Button", this->get_status_button_);
  LOG_BUTTON("  ", "Clear Positioning Button", this->clear_positioning_button_);
  LOG_BUTTON("  ", "Factory Reset Button", this->factory_reset_button_);
#endif
#ifdef USE_SWITCH
  LOG_SWITCH("  ", "Invert Direction Switch", this->invert_direction_switch_);
  LOG_SWITCH("  ", "Pull to Start Switch", this->pull_to_start_switch_);
#endif
}

}  // namespace dooya
}  // namespace esphome
