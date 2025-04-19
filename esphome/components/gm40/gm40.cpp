#include "gm40.h"
#include "esphome/core/log.h"

namespace esphome {
namespace gm40 {

static const char *const TAG = "gm40.cover";

using namespace esphome::cover;

uint8_t calc_checksum(const std::vector<uint8_t> &frame) {
  uint8_t checksum = 0;
  for (auto i = frame.begin() + 3; i != frame.end(); ++i) {
    checksum ^= *i;
  }
  return checksum;
}

CoverTraits GM40Cover::get_traits() {
  auto traits = CoverTraits();
  traits.set_supports_stop(true);
  traits.set_supports_position(true);
  return traits;
}

#ifdef USE_NUMBER
void GM40Number::control(float value) {
  this->parent_->target_speed = value;
  uint8_t target = static_cast<uint8_t>(value);
  ESP_LOGI(TAG, "Setting speed to %u", target);
  uint8_t data[5] = {CONTROL, 0x00, SPEED, 0x00, target};
  this->parent_->send_command(data, 5);
}
#endif

void GM40Cover::control(const CoverCall &call) {
  if (call.get_stop()) {
    uint8_t data[5] = {CONTROL, 0x00, STOP, 0x00, 0x01};
    this->send_command(data, 5);
  } else if (call.get_position().has_value()) {
    this->target_position_ = *call.get_position();
    if ((uint8_t) (this->target_position_ * 100) != (uint8_t) (this->position * 100)) {
      if (this->target_position_ == COVER_OPEN) {
        uint8_t data[5] = {CONTROL, 0x00, OPEN, 0x00, 0x01};
        this->send_command(data, 5);
      } else if (this->target_position_ == COVER_CLOSED) {
        uint8_t data[5] = {CONTROL, 0x00, CLOSE, 0x00, 0x01};
        this->send_command(data, 5);
      } else {
        uint8_t data[5] = {CONTROL, 0x00, POSITION, 0x00, (uint8_t) (100 - (this->target_position_ * 100))};
        this->send_command(data, 5);
      }
    }
  }
}

void GM40Cover::send_update() {
  if (this->current_operation != COVER_OPERATION_IDLE) {
    uint8_t data[5] = {READ, 0x00, POSITION, 0x00, 0x01};
    this->send_command(data, 5);
  }
}

void GM40Cover::on_uart_multi_byte(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  switch (at) {
    case 0:
      if (byte == START_CODE_H)
        this->rx_buffer_.push_back(byte);
      break;
    case 1:
      if (byte == START_CODE_L)
        this->rx_buffer_.push_back(byte);
      break;
    case 2:
      if (byte == 0x04 || byte == 0x06 || byte == 0x09)
        this->rx_buffer_.push_back(byte);
      else
        this->rx_buffer_.clear();
      break;
    case 3:
      if (byte == this->address_)
        this->rx_buffer_.push_back(byte);
      else
        this->rx_buffer_.clear();
      break;
    case 4:
      if (byte == READ || byte == STATUS || byte == CONTROL)
        this->rx_buffer_.push_back(byte);
      else
        this->rx_buffer_.clear();
      break;
    case 5:
    case 10:
      if (byte == 0x00)
        this->rx_buffer_.push_back(byte);
      else
        this->rx_buffer_.clear();
      break;
    case 7:
      if (this->rx_buffer_[4] == CONTROL) {
        if (calc_checksum(this->rx_buffer_) == byte)
          this->process_control_response_();
        else
          ESP_LOGE(TAG, "Checksum failed");
        this->rx_buffer_.clear();
        this->parent_->ready_to_tx = true;
      } else if (byte != 0x00)
        this->rx_buffer_.clear();
        else
        this->rx_buffer_.push_back(byte);
      break;
    case 9:
      if (this->rx_buffer_[4] == STATUS) {
        if (calc_checksum(this->rx_buffer_) == byte)
          this->process_status_();
        else
          ESP_LOGE(TAG, "Checksum failed");
        this->rx_buffer_.clear();
        this->parent_->ready_to_tx = true;
      } else
        this->rx_buffer_.push_back(byte);
      break;
    case 12:
      if (calc_checksum(this->rx_buffer_) == byte)
        this->process_read_response_();
      else
        ESP_LOGE(TAG, "Checksum failed");
      this->rx_buffer_.clear();
      this->parent_->ready_to_tx = true;
      break;
    default:
      this->rx_buffer_.push_back(byte);
  }
}

void GM40Cover::process_control_response_() {
  switch (this->rx_buffer_[6]) {
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
    case POSITION:
      if (this->target_position_ > this->position)
        this->current_operation = COVER_OPERATION_OPENING;
      else
        this->current_operation = COVER_OPERATION_CLOSING;
      this->publish_state(false);
      break;
#ifdef USE_SWITCH
    case INVERT_DIRECTION:
      if (this->invert_direction_switch_ != nullptr)
        this->invert_direction_switch_->publish_state(!this->invert_direction_switch_->state);
      break;
    case LED_INDICATOR:
      if (this->led_indicator_switch_ != nullptr)
        this->led_indicator_switch_->publish_state(!this->led_indicator_switch_->state);
      break;
#endif
#ifdef USE_NUMBER
    case SPEED:
      if (this->speed_number_ != nullptr)
        this->speed_number_->publish_state(this->target_speed);
      break;
#endif
    case FACTORY_RESET:
      ESP_LOGI(TAG, "Factory reset successful");
      break;
    case RESTART:
      ESP_LOGI(TAG, "Restart successful");
      break;
    default:
      ESP_LOGE(TAG, "Invalid control operation received");
      break;
  }
  this->publish_state(false);
}

void GM40Cover::process_read_response_() {
  switch (this->rx_buffer_[6]) {
    case POSITION:
      if (this->rx_buffer_[11] != UNKNOWN_POSITION) {
        if ((uint8_t) (this->position * 100) != 100 - this->rx_buffer_[11])
          this->position = clamp((float) (100 - this->rx_buffer_[11]) / 100, 0.0f, 1.0f);
        else
          this->current_operation = COVER_OPERATION_IDLE;
        this->publish_state(false);
      }
#ifdef USE_BINARY_SENSOR
      if (this->positioning_binary_sensor_ != nullptr)
        this->positioning_binary_sensor_->publish_state(this->rx_buffer_[11] == UNKNOWN_POSITION);
#endif
      break;
#ifdef USE_SWITCH
    case INVERT_DIRECTION:
      if (this->invert_direction_switch_ != nullptr)
        this->invert_direction_switch_->publish_state(this->rx_buffer_[11] == 0x01);
      break;
    case LED_INDICATOR:
      if (this->led_indicator_switch_ != nullptr)
        this->led_indicator_switch_->publish_state(this->rx_buffer_[11] == 0x01);
      break;
#endif
#ifdef USE_NUMBER
    case SPEED:
      if (this->speed_number_ != nullptr)
        this->speed_number_->publish_state((float) this->rx_buffer_[11]);
      break;
#endif
    default:
      ESP_LOGE(TAG, "Invalid read response received");
      break;
  }
}

void GM40Cover::process_status_() {
  if (this->rx_buffer_[6] == POSITION) {
    if ((uint8_t) (this->position * 100) != 100 - this->rx_buffer_[8])
      this->position = clamp((float) (100 - this->rx_buffer_[8]) / 100, 0.0f, 1.0f);
    this->current_operation = COVER_OPERATION_IDLE;
    this->publish_state(false);
  } else
    ESP_LOGE(TAG, "Invalid status received");
}

void GM40Cover::send_command(const uint8_t *data, uint8_t len) {
  std::vector<uint8_t> frame = {this->address_};
  frame.insert(frame.end(), data, data + len);
  uint8_t header[3] = {START_CODE_H, START_CODE_L, (uint8_t) frame.size()};
  frame.push_back(calc_checksum(frame));
  frame.insert(frame.begin(), header, header + 3);

  this->send(frame);
}

void GM40Cover::dump_config() {
  ESP_LOGCONFIG(TAG, "GM40:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
#ifdef USE_BINARY_SENSOR
  LOG_BINARY_SENSOR("  ", "Positioning", this->positioning_binary_sensor_);
#endif
#ifdef USE_BUTTON
  LOG_BUTTON("  ", "Get Status Button", this->get_status_button_);
  LOG_BUTTON("  ", "Factory Reset Button", this->factory_reset_button_);
  LOG_BUTTON("  ", "Restart Button", this->restart_button_);
#endif
#ifdef USE_NUMBER
  LOG_NUMBER("  ", "Speed Number", this->speed_number_);
#endif
#ifdef USE_SWITCH
  LOG_SWITCH("  ", "Invert Direction Switch", this->invert_direction_switch_);
  LOG_SWITCH("  ", "LED Indicator Switch", this->led_indicator_switch_);
#endif
}

}  // namespace gm40
}  // namespace esphome
