#pragma once

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

#include "../gm40.h"

namespace esphome {
namespace gm40 {

class GM40Switch : public switch_::Switch, public Component, public Parented<GM40Cover> {};

class InvertDirectionSwitch : public GM40Switch {
 public:
  void write_state(bool state) override;
};

class LedIndicatorSwitch : public GM40Switch {
 public:
  void write_state(bool state) override;
};

}  // namespace gm40
}  // namespace esphome
