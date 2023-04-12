// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>

namespace V2Base::Cryptography {
class SHA1 {
public:
  void init();
  void update(const uint8_t *data, uint32_t len);
  void final(uint8_t digest[20]);

private:
  uint32_t _state[5];
  uint32_t _count[2];
  unsigned char _buffer[64];

  void transform(uint32_t state[5], const uint8_t buffer[64]);
};
};
