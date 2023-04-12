// © Kay Sievers <kay@versioduo.com>, 2021-2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>
#include <functional>
#include <wiring_private.h>

// // TC3, 1kHz
// V2Base::Timer::Periodic Timer(3, 1000);
//
// Timer.begin([]() { LEDBuiltin.tick(); });
// Timer.begin(std::bind(&V2LED::Basic::tick, &LEDBuiltin));

namespace V2Base::Timer {
class Periodic {
public:
  constexpr Periodic(uint8_t id, uint32_t frequency) : _id{id}, _frequency{frequency} {}
  void begin(std::function<void()> handler);

  // Provide a PWM/duty-cycle-like interrupt pattern; use the second channel to fire
  // at a fraction of the period.
  bool isFraction() {
    return _tc->COUNT16.INTFLAG.bit.MC1;
  }

  void setFraction(float duty) {
    if (duty <= 0.f || duty >= 1) {
      _tc->COUNT16.INTENCLR.reg = TC_INTENCLR_MC1;
      return;
    }

    _tc->COUNT16.CC[1].reg = (float)_period * duty;
    while (_tc->COUNT16.SYNCBUSY.bit.CC1)
      ;

    // Enable the match interrupt for channel 1, the fraction of the period.
    _tc->COUNT16.INTENSET.reg = TC_INTENSET_MC1;
  }

  void enable();
  void disable();

  void setPriority(uint8_t level);

  uint32_t getFrequency() {
    return _frequency;
  }

private:
  const uint8_t _id;
  Tc *_tc{};
  const uint32_t _frequency;
  uint32_t _period{};
};
};
