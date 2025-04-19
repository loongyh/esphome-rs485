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
namespace gm40 {

static const uint8_t START_CODE_H = 0x5A;
static const uint8_t START_CODE_L = 0xA5;
static const uint8_t UNKNOWN_POSITION = 0xFF;

enum Command : uint8_t {
  READ = 0x03,
  STATUS = 0x04,
  CONTROL = 0x06,
};

enum Register : uint8_t {
  OPEN = 0x01,
  CLOSE = 0x02,
  STOP = 0x03,
  POSITION = 0x04,
  INVERT_DIRECTION = 0x11,
  SPEED = 0x12,
  FACTORY_RESET = 0x21,
  RESTART = 0x22,
  LED_INDICATOR = 0x32,
};

class GM40Cover : public cover::Cover, public Component, public uart_multi::UARTMultiDevice {
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
#endif
#ifdef USE_SWITCH
  SUB_SWITCH(invert_direction)
  SUB_SWITCH(led_indicator)
#endif
 public:
  void dump_config() override;
  void set_address(uint8_t address) { this->address_ = address; }
  void send_update() override;
  void on_uart_multi_byte(uint8_t byte) override;
  cover::CoverTraits get_traits() override;
  void send_command(const uint8_t *data, uint8_t len);

  float target_speed;

 protected:
  void control(const cover::CoverCall &call) override;
  void process_control_response_();
  void process_read_response_();
  void process_status_();

  uint8_t address_{0x00};
  std::vector<uint8_t> rx_buffer_;
  float target_position_;
};

#ifdef USE_NUMBER
class GM40Number : public number::Number, public Component, public Parented<GM40Cover> {
 public:
  void control(float value) override;
};
#endif

}  // namespace gm40
}  // namespace esphome
