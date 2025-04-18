#pragma once

#include "esphome/components/button/button.h"
#include "esphome/core/component.h"

#include "../dooya.h"

namespace esphome {
namespace dooya {

class DooyaButton : public button::Button, public Component, public Parented<DooyaCover> {};

class GetStatusButton : public DooyaButton {
 public:
  void press_action() override;
};

class ClearPositioningButton : public DooyaButton {
 public:
  void press_action() override;
};

class FactoryResetButton : public DooyaButton {
 public:
  void press_action() override;
};

}  // namespace dooya
}  // namespace esphome
