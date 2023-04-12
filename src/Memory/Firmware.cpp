// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#include "V2Base.h"

extern uint32_t __text_start__;
uint32_t V2Base::Memory::Firmware::getStart() {
  return (uint32_t)&__text_start__;
}

extern uint32_t __sketch_size;
uint32_t V2Base::Memory::Firmware::getSize() {
  // The '__sketch_size' symbol requires explicit support from the linker script.
  return (uint32_t)&__sketch_size;
}

void V2Base::Memory::Firmware::calculateHash(uint32_t offset, uint32_t len, char hash[41]) {
  V2Base::Cryptography::SHA1 sha;
  sha.init();

  for (uint32_t i = 0; i < len; i++)
    sha.update((uint8_t *)offset + i, 1);

  uint8_t digest[20];
  sha.final(digest);

  for (uint8_t i = 0; i < 20; i++)
    sprintf((char *)hash + (i * 2), "%02x", digest[i]);
}

void V2Base::Memory::Firmware::reboot() {
  NVIC_SystemReset();
}

void V2Base::Memory::Firmware::Secondary::writeBlock(uint32_t offset, const uint32_t *data) {
  const uint32_t start = getStart() + Firmware::getStart();
  if (memcmp((const uint8_t *)start + offset, data, Flash::getBlockSize()) == 0)
    return;

  Flash::writeBlock(start + offset, data);
}

bool V2Base::Memory::Firmware::Secondary::verify(uint32_t firmwareLen, const char *hash) {
  if (memcmp(NULL, (const uint8_t *)getStart(), Firmware::getStart()) != 0)
    return false;

  const uint32_t start = getStart() + Firmware::getStart();
  char secondary[41];
  calculateHash(start, firmwareLen, secondary);
  return strcmp(hash, secondary) == 0;
}

void V2Base::Memory::Firmware::Secondary::copyBootloader() {
  for (uint32_t i = 0; i < Firmware::getStart(); i += Flash::getBlockSize()) {
    if (memcmp((const uint8_t *)i, (const uint8_t *)getStart() + i, Flash::getBlockSize()) == 0)
      continue;

    Flash::writeBlock(getStart() + i, (const uint32_t *)i);
  }
}

void V2Base::Memory::Firmware::Secondary::activate() {
  // Swap the flash banks, reallocate the EEPROM area, reset the system.
  while (!NVMCTRL->STATUS.bit.READY)
    ;
  NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMDEX_KEY | NVMCTRL_CTRLB_CMD_BKSWRST;
}
