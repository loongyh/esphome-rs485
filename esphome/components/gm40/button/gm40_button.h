#pragma once

#include "esphome/components/button/button.h"
#include "esphome/core/component.h"

#include "../gm40.h"

namespace esphome {
namespace gm40 {

class GM40Button : public button::Button, public Component, public Parented<GM40Cover> {};

class GetStatusButton : public GM40Button {
 public:
  void press_action() override;
};

class FactoryResetButton : public GM40Button {
 public:
  void press_action() override;
};

class RestartButton : public GM40Button {
 public:
  void press_action() override;
};

}  // namespace gm40
}  // namespace esphome
