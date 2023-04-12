// © Kay Sievers <kay@versioduo.com>, 2021-2023
// SPDX-License-Identifier: Apache-2.0

#include "PWM.h"

static Tcc *const _tccs[TCC_INST_NUM] = TCC_INSTS;

static constexpr uint8_t _gclkIDs[TCC_INST_NUM] = {
  TCC0_GCLK_ID,
  TCC1_GCLK_ID,
  TCC2_GCLK_ID,
#if (TCC_INST_NUM > 3)
  TCC3_GCLK_ID,
  TCC4_GCLK_ID,
#endif
};

void V2Base::Timer::PWM::begin() {
  _tcc = _tccs[_id];

  // Clock generator 0 == 120MHz.
  GCLK->PCHCTRL[_gclkIDs[_id]].reg = GCLK_PCHCTRL_GEN_GCLK0_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);

  _tcc->CTRLA.bit.SWRST = 1;
  while (_tcc->SYNCBUSY.bit.SWRST)
    ;

  // Normal PWM.
  _tcc->WAVE.reg = TCC_WAVE_WAVEGEN_NPWM;
  while (_tcc->SYNCBUSY.bit.WAVE)
    ;

  // Example:
  // 120MHz, 20 kHz timer == 6000 ticks / period.
  const uint32_t clock = 120000000;
  uint32_t prescaler   = TCC_CTRLA_PRESCALER_DIV1;
  _period              = (clock / 1) / _frequency;
  if (_period > 0xffff) {
    prescaler = TCC_CTRLA_PRESCALER_DIV2;
    _period   = (clock / 2) / _frequency;

  } else if (_period > 0xffff) {
    prescaler = TCC_CTRLA_PRESCALER_DIV4;
    _period   = (clock / 4) / _frequency;

  } else if (_period > 0xffff) {
    prescaler = TCC_CTRLA_PRESCALER_DIV8;
    _period   = (clock / 8) / _frequency;

  } else if (_period > 0xffff) {
    prescaler = TCC_CTRLA_PRESCALER_DIV16;
    _period   = (clock / 16) / _frequency;

  } else if (_period > 0xffff) {
    prescaler = TCC_CTRLA_PRESCALER_DIV64;
    _period   = (clock / 64) / _frequency;

  } else if (_period > 0xffff) {
    prescaler = TCC_CTRLA_PRESCALER_DIV256;
    _period   = (clock / 256) / _frequency;

  } else if (_period > 0xffff) {
    prescaler = TCC_CTRLA_PRESCALER_DIV1024;
    _period   = (clock / 1024) / _frequency;
  }

  _tcc->CTRLA.reg |= prescaler | TCC_CTRLA_PRESCSYNC_GCLK;

  _tcc->PER.reg = _period;
  while (_tcc->SYNCBUSY.bit.PER)
    ;

  // Enable TCC
  _tcc->CTRLA.bit.ENABLE = 1;
  while (_tcc->SYNCBUSY.bit.ENABLE)
    ;
}
