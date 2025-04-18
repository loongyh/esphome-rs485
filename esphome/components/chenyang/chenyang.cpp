#include "chenyang.h"
#include "esphome/core/log.h"

namespace esphome {
namespace chenyang {

static const char *const TAG = "chenyang.cover";

using namespace esphome::cover;

uint8_t calc_checksum(const std::vector<uint8_t> &frame) {
  uint8_t checksum = 0;
  for (auto i : frame) {
    checksum ^= i;
  }
  return checksum;
}

CoverTraits ChenyangCover::get_traits() {
  auto traits = CoverTraits();
  traits.set_supports_stop(true);
  traits.set_supports_position(true);
  return traits;
}

void ChenyangCover::control(const CoverCall &call) {
  if (call.get_stop()) {
    uint8_t data[2] = {STOP, 0x00};
    this->send_command(data, 2);
  } else if (call.get_position().has_value()) {
    this->target_position_ = *call.get_position();
    if (this->target_position_ != this->position) {
      if (this->target_position_ == COVER_OPEN) {
        uint8_t data[2] = {OPEN, 0x00};
        this->send_command(data, 2);
      } else if (this->target_position_ == COVER_CLOSED) {
        uint8_t data[2] = {CLOSE, 0x00};
        this->send_command(data, 2);
      } else {
        uint8_t data[2] = {SET_POSITION, (uint8_t) (this->target_position_ * 100)};
        this->send_command(data, 2);
      }
    }
  }
}

void ChenyangCover::send_update() {
  if (this->current_operation != COVER_OPERATION_IDLE) {
    uint8_t data[2] = {GET_STATUS, 0x00};
    this->send_command(data, 2);
  }
}

void ChenyangCover::on_uart_multi_byte(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  switch (at) {
    case 0:
      if (byte == RESPONSE || byte == STATUS)
        this->rx_buffer_.push_back(byte);
      break;
    case 1:
      if (byte == this->address_ || byte == 0xFF)
        this->rx_buffer_.push_back(byte);
      else
        this->rx_buffer_.clear();
      break;
    case 4:
      if (this->rx_buffer_[0] == RESPONSE) {
        if (byte == calc_checksum(this->rx_buffer_))
          this->process_response_();
        else
          ESP_LOGE(TAG, "Checksum failed");
        this->rx_buffer_.clear();
        this->parent_->ready_to_tx = true;
      } else
        this->rx_buffer_.push_back(byte);
      break;
    case 13:
      if (byte == calc_checksum(this->rx_buffer_))
        this->process_status_();
      else
        ESP_LOGE(TAG, "Checksum failed");
      this->rx_buffer_.clear();
      this->parent_->ready_to_tx = true;
      break;
    default:
      this->rx_buffer_.push_back(byte);
  }
}

void ChenyangCover::process_response_() {
  switch (this->rx_buffer_[2]) {
    case OPEN:
      this->current_operation = COVER_OPERATION_OPENING;
      break;
    case CLOSE:
      this->current_operation = COVER_OPERATION_CLOSING;
      break;
    case STOP:
      this->current_operation = COVER_OPERATION_IDLE;
      break;
    case SET_POSITION:
      if (this->rx_buffer_[3] != UNKNOWN_POSITION) {
        if (this->target_position_ > this->position)
          this->current_operation = COVER_OPERATION_OPENING;
        else
          this->current_operation = COVER_OPERATION_CLOSING;
      }
#ifdef USE_BINARY_SENSOR
      if (this->positioning_binary_sensor_ != nullptr)
        this->positioning_binary_sensor_->publish_state(this->rx_buffer_[3] == UNKNOWN_POSITION);
#endif
      break;
    case FACTORY_RESET:
      ESP_LOGI(TAG, "Factory reset successful");
      break;
    case RESTART:
      ESP_LOGI(TAG, "Restart successful");
      break;
    case RAIN_DETECTED:
      ESP_LOGI(TAG, "Rain detected");
      break;
    case OBSTRUCTION_DETECTED:
      ESP_LOGW(TAG, "Obstruction detected");
      break;
#ifdef USE_NUMBER
    case SPEED:
      if (this->speed_number_ != nullptr)
        this->speed_number_->publish_state((float) this->rx_buffer_[3]);
      break;
    case TORQUE:
      if (this->torque_number_ != nullptr)
        this->torque_number_->publish_state((float) this->rx_buffer_[3]);
      break;
#endif
#ifdef USE_SWITCH
    case LED_INDICATOR:
      if (this->led_indicator_switch_ != nullptr)
        this->led_indicator_switch_->publish_state(this->rx_buffer_[3] == 0x01);
      break;
    case INVERT_DIRECTION:
      if (this->invert_direction_switch_ != nullptr)
        this->invert_direction_switch_->publish_state(this->rx_buffer_[3] == 0x01);
      break;
    case PULL_TO_START:
      if (this->pull_to_start_switch_ != nullptr)
        this->pull_to_start_switch_->publish_state(this->rx_buffer_[3] == 0x00);
      break;
    case CLOSE_ON_POWER_ON:
      if (this->close_on_power_on_switch_ != nullptr)
        this->close_on_power_on_switch_->publish_state(this->rx_buffer_[3] == 0x01);
      break;
    case OPEN_ON_POWER_ON:
      if (this->open_on_power_on_switch_ != nullptr)
        this->open_on_power_on_switch_->publish_state(this->rx_buffer_[3] == 0x01);
      break;
    case RAIN_SENSOR:
      if (this->rain_sensor_switch_ != nullptr)
        this->rain_sensor_switch_->publish_state(this->rx_buffer_[3] == 0x00);
      break;
    case RAIN_INVERT_DIRECTION:
      if (this->rain_invert_direction_switch_ != nullptr)
        this->rain_invert_direction_switch_->publish_state(this->rx_buffer_[3] == 0x01);
      break;
    case LOCK:
      if (this->lock_switch_ != nullptr)
        this->lock_switch_->publish_state(this->rx_buffer_[3] == 0x01);
      break;
    case POWER_OFF_UNLOCK:
      if (this->power_off_unlock_switch_ != nullptr)
        this->power_off_unlock_switch_->publish_state(this->rx_buffer_[3] == 0x01);
      break;
#endif
    default:
      ESP_LOGE(TAG, "Invalid control operation received");
      return;
  }
  this->publish_state(false);
}

void ChenyangCover::process_status_() {
  bool publish_state = false;
  switch (this->rx_buffer_[2]) {
    case OPEN:
      if (this->current_operation != COVER_OPERATION_OPENING) {
        this->current_operation = COVER_OPERATION_OPENING;
        publish_state = true;
      }
      break;
    case CLOSE:
      if (this->current_operation != COVER_OPERATION_CLOSING) {
        this->current_operation = COVER_OPERATION_CLOSING;
        publish_state = true;
      }
      break;
    case STOP:
      if (this->current_operation != COVER_OPERATION_IDLE) {
        this->current_operation = COVER_OPERATION_IDLE;
        publish_state = true;
      }
      break;
    default:
      ESP_LOGE(TAG, "Invalid status operation received");
      return;
  }
  if (this->rx_buffer_[3] != UNKNOWN_POSITION) {
    if ((uint8_t) (this->position * 100) != this->rx_buffer_[3]) {
      this->position = clamp((float) this->rx_buffer_[3] / 100, 0.0f, 1.0f);
      publish_state = true;
    }
  }
  if (publish_state)
    this->publish_state(false);
#ifdef USE_BINARY_SENSOR
  if (this->positioning_binary_sensor_ != nullptr)
    this->positioning_binary_sensor_->publish_state(this->rx_buffer_[3] == UNKNOWN_POSITION);
#endif
#ifdef USE_NUMBER
  if (this->speed_number_ != nullptr) {
    if ((uint8_t) this->speed_number_->state != this->rx_buffer_[4])
      this->speed_number_->publish_state((float) this->rx_buffer_[4]);
  }
    
  if (this->torque_number_ != nullptr) {
    if ((uint8_t) this->torque_number_->state != this->rx_buffer_[5])
      this->torque_number_->publish_state((float) this->rx_buffer_[5]);
  }
#endif
#ifdef USE_SWITCH
  if (this->invert_direction_switch_ != nullptr)
    this->invert_direction_switch_->publish_state(this->rx_buffer_[6] == 0x01);
  if (this->pull_to_start_switch_ != nullptr)
    this->pull_to_start_switch_->publish_state(this->rx_buffer_[7] == 0x00);
  if (this->close_on_power_on_switch_ != nullptr)
    this->close_on_power_on_switch_->publish_state(this->rx_buffer_[8] == 0x01);
  if (this->open_on_power_on_switch_ != nullptr)
    this->open_on_power_on_switch_->publish_state(this->rx_buffer_[9] == 0x01);
  if (this->rain_sensor_switch_ != nullptr)
    this->rain_sensor_switch_->publish_state(this->rx_buffer_[10] == 0x00);
  if (this->rain_invert_direction_switch_ != nullptr)
    this->rain_invert_direction_switch_->publish_state(this->rx_buffer_[11] == 0x01);
  if (this->lock_switch_ != nullptr)
    this->lock_switch_->publish_state(this->rx_buffer_[12] == 0x01);
#endif
}

void ChenyangCover::send_command(const uint8_t *data, uint8_t len) {
  std::vector<uint8_t> frame = {COMMAND, this->address_};
  for (size_t i = 0; i < len; i++) {
    frame.push_back(data[i]);
  }
  frame.push_back(calc_checksum(frame));

  this->send(frame);
}

void ChenyangCover::dump_config() {
  ESP_LOGCONFIG(TAG, "Chenyang:");
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
  LOG_NUMBER("  ", "Torque Number", this->torque_number_);
#endif
#ifdef USE_SWITCH
  LOG_SWITCH("  ", "LED Indicator Switch", this->led_indicator_switch_);
  LOG_SWITCH("  ", "Invert Direction Switch", this->invert_direction_switch_);
  LOG_SWITCH("  ", "Pull to Start Switch", this->pull_to_start_switch_);
  LOG_SWITCH("  ", "Close on Power On Switch", this->close_on_power_on_switch_);
  LOG_SWITCH("  ", "Open on Power On Switch", this->open_on_power_on_switch_);
  LOG_SWITCH("  ", "Rain Sensor Switch", this->rain_sensor_switch_);
  LOG_SWITCH("  ", "Rain Invert Direction Switch", this->rain_invert_direction_switch_);
  LOG_SWITCH("  ", "Lock Switch", this->lock_switch_);
  LOG_SWITCH("  ", "Power Off Unlock Switch", this->power_off_unlock_switch_);
#endif
}

}  // namespace chenyang
}  // namespace esphome
