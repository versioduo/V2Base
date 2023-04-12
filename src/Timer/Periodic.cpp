// © Kay Sievers <kay@versioduo.com>, 2021-2023
// SPDX-License-Identifier: Apache-2.0

#include "Periodic.h"

static Tc *const _tcs[TC_INST_NUM] = TC_INSTS;

static constexpr IRQn_Type _tcIRQs[TC_INST_NUM] = {
  TC0_IRQn,
  TC1_IRQn,
  TC2_IRQn,
  TC3_IRQn,
#if (TC_INST_NUM > 4)
  TC4_IRQn,
  TC5_IRQn,
#endif
};

static constexpr uint8_t _gclkIDs[TC_INST_NUM] = {
  TC0_GCLK_ID,
  TC1_GCLK_ID,
  TC2_GCLK_ID,
  TC3_GCLK_ID,
#if (TC_INST_NUM > 4)
  TC4_GCLK_ID,
  TC5_GCLK_ID,
#endif
};

static std::function<void()> _tcHandlers[TC_INST_NUM]{};

static void TCHandler(uint8_t id) {
  _tcHandlers[id]();
  _tcs[id]->COUNT16.INTFLAG.reg = TC_INTFLAG_MC0 | TC_INTFLAG_MC1;
}

void TC2_Handler() {
  TCHandler(2);
}

void TC3_Handler() {
  TCHandler(3);
}

void V2Base::Timer::Periodic::begin(std::function<void()> handler) {
  _tcHandlers[_id] = handler;
  _tc              = _tcs[_id];

  // Clock generator 0 == 120MHz.
  GCLK->PCHCTRL[_gclkIDs[_id]].reg = GCLK_PCHCTRL_GEN_GCLK0_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);

  _tc->COUNT16.CTRLA.bit.SWRST = 1;
  while (_tc->COUNT16.SYNCBUSY.bit.SWRST)
    ;

  // Match Pulse-Width Modulation Mode to support generating a
  // second interrupt at the fraction of the period.
  _tc->COUNT16.WAVE.bit.WAVEGEN = TC_WAVE_WAVEGEN_MPWM;

  // Enable the match interrupt for the channel 0.
  _tc->COUNT16.INTENSET.reg = TC_INTENSET_MC0;

  // Example:
  // 120MHz/8 == 15Mhz, 500Hz timer == 30000 ticks / period.
  const uint32_t clock = 120000000;
  uint32_t prescaler   = TC_CTRLA_PRESCALER_DIV1;
  _period              = (clock / 1) / _frequency;
  if (_period > 0xffff) {
    prescaler = TC_CTRLA_PRESCALER_DIV8;
    _period   = (clock / 8) / _frequency;

  } else if (_period > 0xffff) {
    prescaler = TC_CTRLA_PRESCALER_DIV16;
    _period   = (clock / 16) / _frequency;

  } else if (_period > 0xffff) {
    prescaler = TC_CTRLA_PRESCALER_DIV64;
    _period   = (clock / 64) / _frequency;

  } else if (_period > 0xffff) {
    prescaler = TC_CTRLA_PRESCALER_DIV256;
    _period   = (clock / 256) / _frequency;

  } else if (_period > 0xffff) {
    prescaler = TC_CTRLA_PRESCALER_DIV1024;
    _period   = (clock / 1024) / _frequency;
  }

  _tc->COUNT16.CTRLA.reg |= prescaler | TCC_CTRLA_PRESCSYNC_GCLK;

  _tc->COUNT16.CC[0].reg = _period;
  while (_tc->COUNT16.SYNCBUSY.bit.CC0)
    ;

  _tc->COUNT16.CTRLA.bit.ENABLE = 1;
  while (_tc->COUNT16.SYNCBUSY.bit.ENABLE)
    ;

  NVIC_EnableIRQ(_tcIRQs[_id]);
}

void V2Base::Timer::Periodic::setPriority(uint8_t level) {
  NVIC_SetPriority(_tcIRQs[_id], level);
}

void V2Base::Timer::Periodic::enable() {
  NVIC_EnableIRQ(_tcIRQs[_id]);
}

void V2Base::Timer::Periodic::disable() {
  NVIC_DisableIRQ(_tcIRQs[_id]);
}
