// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>

namespace V2Base::Memory {
class Flash {
public:
  static constexpr uint32_t getSize() {
    return FLASH_SIZE;
  }
  static constexpr uint32_t getPageSize() {
    return NVMCTRL_PAGE_SIZE;
  }
  static constexpr uint32_t getBlockSize() {
    return NVMCTRL_BLOCK_SIZE;
  }

  // Returns the currently active flash bank. Firmware::Secondary::activate swaps them.
  static uint8_t getBank();

  // The offset needs to be at a page boundary, data needs to be a full page (e.g. 512).
  static void writePage(uint32_t offset, const uint32_t *data);

  // The offset needs to be at a block boundary, data needs to be a full block (e.g. 8k).
  static void eraseBlock(uint32_t offset);
  static void writeBlock(uint32_t offset, const uint32_t *data);

  // "The size of the NVM User Page is 512 Bytes. The first eight 32-bit words (32 Bytes)
  // of the Non Volatile Memory (NVM) User Page contain calibration data that are automatically
  // read at device power on. The remaining 480 Bytes can be used for storing custom parameters."
  class UserPage {
  public:
    static constexpr uint32_t getStart() {
      return NVMCTRL_USER;
    }
    static void read(uint32_t data[128]);
    static void write(uint32_t data[128]);

    // Set the device fuses at the first bootup with a new set of configuration
    // values. V2Device requires EEPROM emulation storage for its configuration.
    static bool update();
  };
};
};
