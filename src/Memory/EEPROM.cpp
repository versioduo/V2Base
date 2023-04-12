// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#include "EEPROM.h"

uint32_t V2Base::Memory::EEPROM::getSize(void) {
  const uint8_t psz  = NVMCTRL->SEESTAT.bit.PSZ;
  const uint8_t sblk = NVMCTRL->SEESTAT.bit.SBLK;

  if (sblk == 0)
    return 0;

  // There are 128 vitual pages of the specified page size. With
  // optimal settings, this is the size of the useable memory area.
  uint32_t virtualPageSize = 4 << psz;
  uint32_t size            = 128 * virtualPageSize;

  // The allocated size of one sector (there are two). If the size of
  // the sector is smaller, shrink the available area until it and its
  // metadata fit into it.
  uint32_t allocated = sblk * NVMCTRL_BLOCK_SIZE;
  while (size >= allocated)
    size /= 2;

  return size;
}

uint32_t V2Base::Memory::EEPROM::getSizeAllocated() {
  const uint8_t sblk = NVMCTRL->SEESTAT.bit.SBLK;

  if (sblk == 0)
    return 0;

  // The allocated size of the two sectors
  return 2 * sblk * NVMCTRL_BLOCK_SIZE;
}

void V2Base::Memory::EEPROM::prepareWrite() {
  NVMCTRL->SEECFG.reg = NVMCTRL_SEECFG_WMODE_BUFFERED;
  while (NVMCTRL->SEESTAT.bit.BUSY)
    ;
}

void V2Base::Memory::EEPROM::flush() {
  while (!NVMCTRL->STATUS.bit.READY)
    ;
  NVMCTRL->CTRLB.reg = NVMCTRL_CTRLB_CMD_SEEFLUSH | NVMCTRL_CTRLB_CMDEX_KEY;
  while (!NVMCTRL->INTFLAG.bit.DONE)
    ;
}

void V2Base::Memory::EEPROM::write(uint32_t offset, const uint8_t *buffer, uint32_t size) {
  // The EEPROM expects 8, 16, 32 bit writes, memcpy() fails.
  prepareWrite();

  uint8_t *data = (uint8_t *)getStart();
  for (uint32_t i = 0; i < size; i++)
    data[offset + i] = buffer[i];

  flush();
}

void V2Base::Memory::EEPROM::erase() {
  uint8_t *data = (uint8_t *)getStart();
  for (uint32_t i = 0; i < getSize(); i++)
    data[i] = 0xff;

  flush();
}
