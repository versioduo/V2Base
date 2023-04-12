// © Kay Sievers <kay@versioduo.com>, 2021-2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>

namespace V2Base::Analog {
class ADC {
public:
  constexpr ADC(uint8_t id) : _id{id} {}

  void begin();

  // Get ADC instance from the pin configuration.
  static uint8_t getID(uint8_t pin) {
    if (g_APinDescription[pin].ulPinAttribute & PIN_ATTR_ANALOG)
      return 0;

    else if (g_APinDescription[pin].ulPinAttribute & PIN_ATTR_ANALOG_ALT)
      return 1;

    while (true)
      ;
  }

  // Get ADC channel number from the pin configuration.
  static uint8_t getChannel(uint8_t pin) {
    return g_APinDescription[pin].ulADCChannelNumber;
  }

  // Switch ADC to a single channel and sample it contiuously.
  void sampleChannel(uint8_t ch);

  // Read the result of the selected channel.
  float read() {
    return (float)_adc->RESULT.reg / 4095.f;
  }

  // Add a channel to the list of channels to continuously read from interrupt context.
  // A single channel is more efficiently measured with setContinuous() + read().
  void addChannel(uint8_t ch);

  // Read the channel's stored value.
  float readChannel(uint8_t ch);

private:
  uint8_t _id;
  Adc *_adc{};
};
};
