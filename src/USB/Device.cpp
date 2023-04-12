// © Kay Sievers <kay@versioduo.com>, 2021-2023
// SPDX-License-Identifier: Apache-2.0

#include "Device.h"

extern "C" {
static uint32_t _connectionSequence{};

// Invoked by tinyUSB when the device is configured by the host.
void tud_mount_cb(void) {
  _connectionSequence++;
}
}

uint32_t V2Base::USBDevice::getConnectionSequence() {
  return _connectionSequence;
}

void V2Base::USBDevice::readSerial(char *serial) {
  uint8_t id[16] __attribute__((aligned(4)));
  TinyUSB_Port_GetSerialNumber(id);
  for (uint8_t i = 0; i < 16; i++)
    sprintf(serial + (i * 2), "%02X", id[i]);
}

bool V2Base::USBDevice::send(uint8_t packet[4]) {
  _device.usec = micros();
  if (!connected())
    return false;

  return _midi.interface.writePacket(packet);
}

bool V2Base::USBDevice::receive(uint8_t packet[4]) {
  if (!connected())
    return false;

  if (!_midi.interface.readPacket(packet))
    return false;

  _device.usec = micros();
  return true;
}
