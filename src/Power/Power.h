// © Kay Sievers <kay@versioduo.com>, 2020-2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>

namespace V2Base {
class Power {
public:
  enum Mode { Idle };

  static void setSleepMode(V2Base::Power::Mode mode) {
    switch (mode) {
      case Mode::Idle:
        // Sleep mode IDLE, calling __WFI() stops the CPU, peripherals are still running.
        PM->SLEEPCFG.bit.SLEEPMODE = 0x02;
        while (PM->SLEEPCFG.bit.SLEEPMODE != 0x02)
          ;
        break;
    }
  }

  static void sleep() {
    __WFI();
  }
};
};
