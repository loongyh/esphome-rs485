#pragma once

#include "esphome/components/button/button.h"
#include "esphome/core/component.h"

#include "../chenyang.h"

namespace esphome {
namespace chenyang {

class ChenyangButton : public button::Button, public Component, public Parented<ChenyangCover> {};

class GetStatusButton : public ChenyangButton {
 public:
  void press_action() override;
};

class FactoryResetButton : public ChenyangButton {
 public:
  void press_action() override;
};

class RestartButton : public ChenyangButton {
 public:
  void press_action() override;
};

}  // namespace chenyang
}  // namespace esphome
