// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#include "ADC.h"

static Adc *const _adcs[] = ADC_INSTS;

static constexpr IRQn_Type _adcIRQS[ADC_INST_NUM] = {
  ADC0_1_IRQn,
  ADC1_1_IRQn,
};

static constexpr uint8_t _gclkIDs[ADC_INST_NUM] = {
  ADC0_GCLK_ID,
  ADC1_GCLK_ID,
};

static struct {
  bool ready{};
  uint16_t mask{};
  uint8_t current{};
  uint16_t results[16];
} _adcChannels[ADC_INST_NUM]{};

static void ADCResultHandler(uint8_t id) {
  // Discard the first measurement after enabling the ADC or swiching the channel.
  if (!_adcChannels[id].ready) {
    // The result is not read, clear the interrupt flag.
    _adcs[id]->INTFLAG.reg = ADC_INTFLAG_RESRDY;

    _adcs[id]->SWTRIG.bit.START = 1;
    _adcChannels[id].ready      = true;
    return;
  }

  _adcChannels[id].results[_adcChannels[id].current] = _adcs[id]->RESULT.reg;

  // Find the next channel to measure.
  for (;;) {
    _adcChannels[id].current++;
    if (_adcChannels[id].current == 16)
      _adcChannels[id].current = 0;

    if (_adcChannels[id].mask & (1 << _adcChannels[id].current))
      break;
  }

  // Select the channel and start the measurement;
  _adcs[id]->INPUTCTRL.bit.MUXPOS = _adcChannels[id].current;
  while (_adcs[id]->SYNCBUSY.bit.INPUTCTRL)
    ;

  _adcs[id]->SWTRIG.bit.START = 1;
  _adcChannels[id].ready      = false;
}

// RESRDY interrupts.
void ADC0_1_Handler() {
  ADCResultHandler(0);
}

void ADC1_1_Handler() {
  ADCResultHandler(1);
}

void V2Base::Analog::ADC::begin() {
  _adc = _adcs[_id];

  // Clock generator 1 == 48MHz.
  GCLK->PCHCTRL[_gclkIDs[_id]].reg = GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);

  _adc->CTRLA.bit.SWRST = 1;
  while (_adc->SYNCBUSY.bit.SWRST)
    ;

  // 48MHz / 32 == 1.5MHz.
  _adc->CTRLA.bit.PRESCALER = ADC_CTRLA_PRESCALER_DIV32_Val;

  // Sampling clock cycles. 1.5MHz / (8 + 12bits) == 75kHz sample rate (The first sample after
  // switching a channel is discarded).
  _adc->SAMPCTRL.reg = 8;
  while (_adc->SYNCBUSY.reg & ADC_SYNCBUSY_SAMPCTRL)
    ;

  // Reference VDDANA == 3V3.
  _adc->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1_Val;
  while (_adc->SYNCBUSY.reg & ADC_SYNCBUSY_REFCTRL)
    ;

  // Positive input only, internal ground.
  _adc->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND;
  while (_adc->SYNCBUSY.reg & ADC_SYNCBUSY_INPUTCTRL)
    ;

  // Enable the ADC.
  _adc->CTRLA.bit.ENABLE = 1;
  while (_adc->SYNCBUSY.bit.ENABLE)
    ;
}

void V2Base::Analog::ADC::sampleChannel(uint8_t ch) {
  _adc->INPUTCTRL.bit.MUXPOS = ch;
  while (_adc->SYNCBUSY.bit.INPUTCTRL)
    ;

  // Sample continuously.
  _adc->CTRLB.bit.FREERUN = 1;
  while (_adc->SYNCBUSY.bit.CTRLB)
    ;

  // Start the conversion, it will never stop.
  _adc->SWTRIG.bit.START = 1;
}

void V2Base::Analog::ADC::addChannel(uint8_t ch) {
  _adcChannels[_id].mask |= 1 << ch;

  NVIC_EnableIRQ(_adcIRQS[_id]);
  NVIC_SetPriority(_adcIRQS[_id], 3);
  _adc->INTENSET.reg = ADC_INTENSET_RESRDY;

  // Start the conversion, the interrupt will start the next.
  _adc->SWTRIG.bit.START = 1;
}

float V2Base::Analog::ADC::readChannel(uint8_t ch) {
  return (float)_adcChannels[_id].results[ch] / 4095.f;
}
