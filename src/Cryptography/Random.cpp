// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#include "Random.h"

static bool _init{};
static volatile uint32_t _data{};

void TRNG_Handler() {
  _data                     = TRNG->DATA.reg;
  TRNG->INTFLAG.bit.DATARDY = 1;
}

uint32_t V2Base::Cryptography::Random::read() {
  if (!_init) {
    _init = true;
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_TRNG;
    TRNG->INTENCLR.reg = TRNG_INTENCLR_DATARDY;
    TRNG->CTRLA.reg    = TRNG_CTRLA_ENABLE;
    NVIC_SetPriority(TRNG_IRQn, 3);
    NVIC_EnableIRQ(TRNG_IRQn);
  }

  // Enable interrupt.
  TRNG->INTENSET.reg = TRNG_INTENSET_DATARDY;

  // Wait for new random data.
  const uint32_t old = _data;
  while (old == _data)
    ;

  // Disable interrupt.
  TRNG->INTENCLR.reg = TRNG_INTENCLR_DATARDY;
  return _data;
}
