// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Analog/ADC.h"
#include "Cryptography/Random.h"
#include "Cryptography/SHA1.h"
#include "GPIO/GPIO.h"
#include "Memory/EEPROM.h"
#include "Memory/Firmware.h"
#include "Memory/Flash.h"
#include "Memory/RAM.h"
#include "Power/Power.h"
#include "Text/Base64.h"
#include "Timer/PWM.h"
#include "Timer/Periodic.h"
#include "USB/Device.h"

namespace V2Base {
template <class T, size_t N> constexpr size_t countof(T (&)[N]) {
  return N;
}

static inline uint32_t getUsec() {
  return micros();
}

static inline uint32_t getUsecSince(uint32_t since) {
  return (uint32_t)(micros() - since);
}
};
