#pragma once

#include "esphome/core/component.h"
#include "esphome/components/cover/cover.h"
#include "esphome/components/uart_multi/uart_multi.h"

#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif

namespace esphome {
namespace dooya {

static const uint8_t START_CODE = 0x55;
static const uint8_t UNKNOWN_POSITION = 0xFF;

enum Command : uint8_t {
  READ = 0x01,
  WRITE = 0x02,
  CONTROL = 0x03,
};

enum ReadType : uint8_t {
  GET_POSITION = 0x02,
  GET_STATUS = 0x05,
};

enum ControlType : uint8_t {
  OPEN = 0x01,
  CLOSE = 0x02,
  STOP = 0x03,
  SET_POSITION = 0x04,
  CLEAR_POSITIONING = 0x07,
  FACTORY_RESET = 0x08,
};

enum SettingType : uint8_t {
  INVERT_DIRECTION = 0x03,
  PULL_TO_START = 0x04,
};

class DooyaCover : public cover::Cover, public Component, public uart_multi::UARTMultiDevice {
#ifdef USE_BINARY_SENSOR
  SUB_BINARY_SENSOR(positioning)
#endif
#ifdef USE_BUTTON
  SUB_BUTTON(get_status)
  SUB_BUTTON(clear_positioning)
  SUB_BUTTON(factory_reset)
#endif
#ifdef USE_SWITCH
  SUB_SWITCH(led_indicator)
  SUB_SWITCH(invert_direction)
  SUB_SWITCH(pull_to_start)
  SUB_SWITCH(close_on_power_on)
  SUB_SWITCH(open_on_power_on)
  SUB_SWITCH(rain_sensor)
  SUB_SWITCH(rain_invert_direction)
  SUB_SWITCH(lock)
  SUB_SWITCH(power_off_unlock)
#endif

 public:
  void setup() override;
  void dump_config() override;
  void set_address(uint16_t address) {
    this->address_[0] = (uint8_t)(address >> 8);
    this->address_[1] = (uint8_t)(address & 0xFF);
  }
  void send_update() override;
  void on_uart_multi_byte(uint8_t byte) override;
  cover::CoverTraits get_traits() override;
  void send_command(const uint8_t *data, uint8_t len);

 protected:
  void control(const cover::CoverCall &call) override;
  void process_read_response_();
  void process_write_response_();
  void process_control_response_();

  uint8_t address_[2] = {0xFE, 0xFE};
  std::vector<uint8_t> rx_buffer_;
  float target_position_;
  uint8_t current_read_request_;
  uint8_t current_write_payload_;
};

}  // namespace dooya
}  // namespace esphome
