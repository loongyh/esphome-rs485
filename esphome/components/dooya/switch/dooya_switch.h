#pragma once

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

#include "../dooya.h"

namespace esphome {
namespace dooya {

class DooyaSwitch : public switch_::Switch, public Component, public Parented<DooyaCover> {};

class InvertDirectionSwitch : public DooyaSwitch {
 public:
  void write_state(bool state) override;
};

class PullToStartSwitch : public DooyaSwitch {
 public:
  void write_state(bool state) override;
};

}  // namespace dooya
}  // namespace esphome
