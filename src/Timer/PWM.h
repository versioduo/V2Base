// © Kay Sievers <kay@versioduo.com>, 2021-2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>
#include <wiring_private.h>

// // Setup the TCC from the pin number, 20kHz.
// V2Base::Timer::PWM PWM(V2Base::Timer::PWM::getID(PIN_PWM), 20000);
//
// // Switch the pin to the TCC output.
// V2Base::Timer::PWM::setupPin(PIN_PWM);
//
// PWM.begin();

namespace V2Base::Timer {
class PWM {
public:
  constexpr PWM(uint8_t id, uint32_t frequency) : _id{id}, _frequency{frequency} {}
  void begin();

  void setDuty(uint8_t pin, float duty) {
    // Double-buffer the new value; it gets only copied to CC with the next
    // transition, to avoid a possible counter wraparound.
    _tcc->CCBUF[getChannel(pin)].reg = (float)_period * duty;
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
