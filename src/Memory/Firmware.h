// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flash.h"
#include <Arduino.h>

namespace V2Base::Memory {
class Firmware {
public:
  // The start of the firmware area, the end of the bootloader area.
  static uint32_t getStart();

  // The size of the current firmware area, excluding the bootloader.
  static uint32_t getSize();

  // Calculate the hash of the given memory area.
  static void calculateHash(uint32_t offset, uint32_t len, char hash[41]);

  static void reboot();

  class Secondary {
  public:
    // The secondary flash bank, mapped to the second half of the flash area.
    static constexpr uint32_t getStart() {
      return Flash::getSize() / 2;
    }

    // Write a firmware block into the secondary flash bank, the offset starts after the bootloader area.
    static void writeBlock(uint32_t offset, const uint32_t *data);

    // Copy the current bootloader to the secondary flash bank.
    static void copyBootloader();

    // Check if the bootloader is identical, and the secondary firmware matches the given hash.
    static bool verify(uint32_t firmwareLen, const char *hash);

    // Swap the flash banks, reallocate the EEPROM area, reset the system.
    static void activate();
  };
};
};
