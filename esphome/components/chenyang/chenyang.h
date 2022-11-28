#pragma once

#include "esphome/core/component.h"
#include "esphome/components/cover/cover.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/uart_multi/uart_multi.h"

namespace esphome {
namespace chenyang {

static const uint8_t UNKNOWN_POSITION = 0x6E;

enum StartCode : uint8_t {
  COMMAND = 0x01,
  RESPONSE = 0x02,
  STATUS = 0x03,
};

enum ReadType : uint8_t {
  GET_STATUS = 0x0F,
};

enum ControlType : uint8_t {
  OPEN = 0x00,
  CLOSE = 0x01,
  STOP = 0x02,
  SET_POSITION = 0x03,
};

class Chenyang : public cover::Cover, public Component, public uart_multi::UARTMultiDevice {
 public:
  void dump_config() override;

  void set_address(uint8_t address) { this->address_ = address; }
  void set_unknown_position_binary_sensor(binary_sensor::BinarySensor *unknown_position_binary_sensor) {
    this->unknown_position_binary_sensor_ = unknown_position_binary_sensor;
  }
  void send_update() override;
  void on_uart_multi_byte(uint8_t byte) override;
  cover::CoverTraits get_traits() override;

 protected:
  void control(const cover::CoverCall &call) override;
  void process_response_();
  void process_status_();
  void send_command_(const uint8_t *data, uint8_t len);

  uint8_t address_{0xFF};
  binary_sensor::BinarySensor *unknown_position_binary_sensor_{nullptr};
  std::vector<uint8_t> rx_buffer_;
  float target_position_{0};
};

}  // namespace chenyang
}  // namespace esphome
