// © Kay Sievers <kay@versioduo.com>, 2021-2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>
#include <wiring_private.h>

// // TCC from pin, 20kHz
// V2Base::Timer::PWM PWM(V2Base::Timer::PWM::getID(PIN_PWM), 20000);
//
// // Switch pin to TCC
// V2Base::Timer::PWM::setupPin(PIN_PWM);
//
// PWM.begin();

namespace V2Base::Timer {
class PWM {
public:
  constexpr PWM(uint8_t id, uint32_t frequency) : _id{id}, _frequency{frequency} {}
  void begin();

  void setDuty(uint8_t pin, float duty) {
    _tcc->CC[getChannel(pin)].reg = (float)_period * duty;
    while (_tcc->SYNCBUSY.reg & TCC_SYNCBUSY_CC(1 << getChannel(pin)))
      ;
  }

  static uint8_t getID(uint8_t pin) {
    return GetTCNumber(g_APinDescription[pin].ulTCChannel);
  }

  static uint8_t getChannel(uint8_t pin) {
    return GetTCChannelNumber(g_APinDescription[pin].ulTCChannel);
  }

  static void setupPin(uint8_t pin) {
    pinPeripheral(pin, g_APinDescription[pin].ulPinType);
  }

private:
  const uint8_t _id{};
  const uint32_t _frequency{};
  Tcc *_tcc{};
  uint32_t _period{};
};
};
