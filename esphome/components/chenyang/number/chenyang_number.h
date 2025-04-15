#pragma once

#include "esphome/components/number/number.h"
#include "esphome/core/component.h"

#include "../chenyang.h"

namespace esphome {
namespace chenyang {

class ChenyangNumber : public number::Number, public Component, public Parented<ChenyangCover> {};

class SpeedNumber : public ChenyangNumber {
 public:
  void control(float value) override;
};

class TorqueNumber : public ChenyangNumber {
 public:
  void control(float value) override;
};

}  // namespace chenyang
}  // namespace esphome
