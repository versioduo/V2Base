// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>

namespace V2Base::Memory {
class RAM {
public:
  static constexpr uint32_t getSize() {
    return HSRAM_SIZE;
  }
  static uint32_t getFree();
};
};
