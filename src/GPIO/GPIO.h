// © Kay Sievers <kay@versioduo.com>, 2021-2022
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>
#include <wiring_private.h>

namespace V2Base {
class GPIO {
public:
  constexpr GPIO(uint8_t pin) :
    _group(&PORT->Group[g_APinDescription[pin].ulPort]),
    _mask((uint32_t)1 << g_APinDescription[pin].ulPin) {}

  void high() {
    _group->OUTSET.reg = _mask;
  }

  void low() {
    _group->OUTCLR.reg = _mask;
  }

private:
  PortGroup *const _group{};
  const uint32_t _mask{};
};
};
