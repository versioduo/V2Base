// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>

namespace V2Base::Memory {
class EEPROM {
public:
  // The base address of the EEPROM data.
  static constexpr uint32_t getStart() {
    return SEEPROM_ADDR;
  }

  // Virtual/useable size of the EEPROM area.
  static uint32_t getSize();

  // Allocated flash space to emulate the EEPROM.
  static uint32_t getSizeAllocated();

  // Enables the SEEPROM buffered mode. Touched pages are only written to flash
  // when the write crosses a page boundary. flush() needs to be called to write
  // the data to the flash.
  //
  // Waits until the SEEPROM becomes ready for writing.
  static void prepareWrite();

  // Flushes the buffered data (current page) to the flash.
  static void flush();

  static void write(uint32_t offset, const uint8_t *buffer, uint32_t size);

  // Overwrite the entire flash area with 0xff.
  static void erase();
};
};
