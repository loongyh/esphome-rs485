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
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif

namespace esphome {
namespace chenyang {

static const uint8_t UNKNOWN_POSITION = 0x6E;

enum StartCode : uint8_t {
  COMMAND = 0x01,
  RESPONSE = 0x02,
  STATUS = 0x03,
};

enum CommandCode : uint8_t {
  OPEN = 0x00,
  CLOSE = 0x01,
  STOP = 0x02,
  SET_POSITION = 0x03,
  SPEED = 0x04,
  TORQUE = 0x05,
  LED_INDICATOR = 0x07,
  INVERT_DIRECTION = 0x08,
  PULL_TO_START = 0x09,
  CLOSE_ON_POWER_ON = 0x0A,
  OPEN_ON_POWER_ON = 0x0B,
  RAIN_SENSOR = 0x0C,
  RAIN_INVERT_DIRECTION = 0x0D,
  GET_STATUS = 0x0F,
  FACTORY_RESET = 0x10,
  RESTART = 0x11,
  LOCK = 0x1A,
  POWER_OFF_UNLOCK = 0x1B,
};

enum DetectionCode: uint8_t {
  RAIN_DETECTED = 0x18,
  OBSTRUCTION_DETECTED = 0x19,
};

class ChenyangCover : public cover::Cover, public Component, public uart_multi::UARTMultiDevice {
#ifdef USE_BINARY_SENSOR
  SUB_BINARY_SENSOR(positioning)
#endif
#ifdef USE_BUTTON
  SUB_BUTTON(get_status)
  SUB_BUTTON(factory_reset)
  SUB_BUTTON(restart)
#endif
#ifdef USE_NUMBER
  SUB_NUMBER(speed)
  SUB_NUMBER(torque)
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
  void dump_config() override;
  void set_address(uint8_t address) { this->address_ = address; }
  void send_update() override;
  void on_uart_multi_byte(uint8_t byte) override;
  cover::CoverTraits get_traits() override;
  void send_command(const uint8_t *data, uint8_t len);

 protected:
  void control(const cover::CoverCall &call) override;
  void process_response_();
  void process_status_();

  uint8_t address_{0xFF};
  std::vector<uint8_t> rx_buffer_;
  float target_position_{0};
};

}  // namespace chenyang
}  // namespace esphome
