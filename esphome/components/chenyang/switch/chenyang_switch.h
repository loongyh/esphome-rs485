#pragma once

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

#include "../chenyang.h"

namespace esphome {
namespace chenyang {

class ChenyangSwitch : public switch_::Switch, public Component, public Parented<ChenyangCover> {};

class LedIndicatorSwitch : public ChenyangSwitch {
 public:
  void write_state(bool state) override;
};

class InvertDirectionSwitch : public ChenyangSwitch {
 public:
  void write_state(bool state) override;
};

class PullToStartSwitch : public ChenyangSwitch {
 public:
  void write_state(bool state) override;
};

class CloseOnPowerOnSwitch : public ChenyangSwitch {
 public:
  void write_state(bool state) override;
};

class OpenOnPowerOnSwitch : public ChenyangSwitch {
 public:
  void write_state(bool state) override;
};

class RainSensorSwitch : public ChenyangSwitch {
 public:
  void write_state(bool state) override;
};

class RainInvertDirectionSwitch : public ChenyangSwitch {
 public:
  void write_state(bool state) override;
};

class LockSwitch : public ChenyangSwitch {
 public:
  void write_state(bool state) override;
};

class PowerOffUnlockSwitch : public ChenyangSwitch {
 public:
  void write_state(bool state) override;
};

}  // namespace chenyang
}  // namespace esphome
