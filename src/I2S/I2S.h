// © Kay Sievers <kay@versioduo.com>, 2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>

namespace V2Base {
class I2SInterface {
public:
  // Append "Interface", CMSIS-Atmel messes with global "#define I2S".
  constexpr I2SInterface(uint8_t pinSCK, uint8_t pinFS, uint8_t pinSD, uint8_t pinMCK) :
    _pin{.sck{pinSCK}, .fs{pinFS}, .sd{pinSD}, .mck{pinMCK}} {}

  void begin() {
    pinPeripheral(_pin.sck, PIO_I2S);
    pinPeripheral(_pin.fs, PIO_I2S);
    pinPeripheral(_pin.sd, PIO_I2S);
    pinPeripheral(_pin.mck, PIO_I2S);

    // Use the 48 Mhz generic clock.
    I2S->CTRLA.bit.ENABLE = 0;
    MCLK->APBDMASK.reg |= MCLK_APBDMASK_I2S;
    GCLK->PCHCTRL[I2S_GCLK_ID_0].reg = GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
    GCLK->PCHCTRL[I2S_GCLK_ID_1].reg = GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);

    const uint32_t sampleRate = 44100;
    const uint32_t bits       = 32;
    const uint32_t numSlots   = 2;
    const uint32_t mckMult    = 64;

    // The Host Clock divider (MCLK). These values result in the closest match to the next integer.
    // 48 Mhz / (44.1 kHz * 64) = 17.0068
    const uint32_t mckFreq   = sampleRate * mckMult;
    const uint32_t mckoutdiv = 48000000 / mckFreq;

    // The Serial Clock divider (SCK). MCLK and SCK need to have the same phase.
    // 48 Mhz / (2 * 32 * 44.1 kHz) = 17.0068
    const uint32_t frameSize = numSlots * bits;
    const uint32_t sckFreq   = sampleRate * frameSize;
    const uint32_t mckdiv    = 48000000 / sckFreq;
    _sampleRate              = 48000000.f / (float)(mckdiv * frameSize);

    I2S->CTRLA.bit.SWRST = 1;
    while (I2S->SYNCBUSY.bit.SWRST)
      ;

    I2S->CLKCTRL[0].reg = I2S_CLKCTRL_MCKSEL_GCLK |
                          I2S_CLKCTRL_MCKOUTDIV(mckoutdiv - 1) |             // Host Clock divider (MCKOUTDIV + 1)
                          I2S_CLKCTRL_MCKDIV(mckdiv - 1) |                   // Serial Clock divider (MCKDIV + 1)
                          I2S_CLKCTRL_SCKSEL_MCKDIV |                        // Divided Host Clock as Serial Clock
                          I2S_CLKCTRL_MCKEN |                                // Host Clock Enable
                          I2S_CLKCTRL_FSSEL_SCKDIV |                         // Divided Serial Clock for Frame Sync
                          I2S_CLKCTRL_BITDELAY_LJ |                          // Left Justified (0 Bit Delay)
                          I2S_CLKCTRL_FSWIDTH_HALF |                         // Frame Sync Pulse is half a Frame wide
                          I2S_CLKCTRL_NBSLOTS(numSlots - 1) |                // 2 slots / Stereo
                          I2S_CLKCTRL_SLOTSIZE(I2S_CLKCTRL_SLOTSIZE_32_Val); // 32 bit wide slots

    I2S->TXCTRL.reg = I2S_TXCTRL_DMA_SINGLE |                           // Single DMA channel
                      I2S_TXCTRL_MONO_STEREO |                          // Normal / Stereo output
                      I2S_TXCTRL_BITREV_MSBIT |                         // Transfer Data MSB first
                      I2S_TXCTRL_WORDADJ_LEFT |                         // Data is left adjusted in word
                      I2S_TXCTRL_DATASIZE(I2S_TXCTRL_DATASIZE_32_Val) | // Data Word Size
                      I2S_TXCTRL_SLOTADJ_LEFT |                         // Data Slot Formatting Adjust
                      I2S_TXCTRL_TXSAME_ZERO;                           // Transmit Data when Underrun

    I2S->CTRLA.bit.ENABLE = 1;
    while (I2S->SYNCBUSY.bit.ENABLE)
      ;

    I2S->CTRLA.bit.CKEN0 = 1;
    while (I2S->SYNCBUSY.bit.CKEN0)
      ;

    I2S->CTRLA.bit.TXEN = 1;
    while (I2S->SYNCBUSY.bit.TXEN)
      ;
  }

  const float getSamplerate() const {
    return _sampleRate;
  }

  bool txReady() const {
    return I2S->INTFLAG.bit.TXRDY0 && !I2S->SYNCBUSY.bit.TXDATA;
  }

  void write(int32_t left, int32_t right) {
    while (!txReady())
      ;
    I2S->INTFLAG.bit.TXUR0 = 1;
    I2S->TXDATA.reg        = left;

    while (!txReady())
      ;
    I2S->INTFLAG.bit.TXUR0 = 1;
    I2S->TXDATA.reg        = right;
  }

private:
  const struct {
    uint8_t sck;
    uint8_t fs;
    uint8_t sd;
    uint8_t mck;
  } _pin;
  float _sampleRate{};
};
};
