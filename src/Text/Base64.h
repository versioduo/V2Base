// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>

namespace V2Base::Text {
class Base64 {
public:
  static uint32_t encode(const uint8_t input[], uint32_t length, uint8_t output[]);
  static uint32_t decode(const uint8_t input[], uint8_t output[]);
};
};
