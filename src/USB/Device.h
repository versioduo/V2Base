// © Kay Sievers <kay@versioduo.com>, 2023
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#undef USBDevice

namespace V2Base {
  class USBDevice {
  public:
    void begin() {
      _device.device->setConfigurationBuffer(_device.descriptor, sizeof(_device.descriptor));
    }

    void setVIDPID(uint16_t vid, uint16_t pid) {
      _device.device->setID(vid, pid);
    }

    void setVendor(const char* name) {
      _device.device->setManufacturerDescriptor(name);
    }

    void setName(const char* name) {
      _device.device->setProductDescriptor(name);
    }

    void setVersion(uint16_t version) {
      // USB uses two two-digit BCD numbers; version 1 will be shown as 0.01, version 815 as 8.15.
      uint16_t bcd = 0;
      for (uint8_t shift = 0; shift < 16; shift += 4) {
        bcd += (version % 10) << shift;
        version /= 10;
      }

      _device.device->setDeviceVersion(bcd);
    }

    void setConfigureURL(const char* url, const char* name) {
      _webUSB.descriptor.bLength += snprintf(_webUSB.descriptor.url,
                                             sizeof(_webUSB.descriptor.url),
                                             "%s?connect=%s",
                                             url + 8,
                                             name);
      _webUSB.interface.begin();
      _webUSB.interface.setLandingPage(&_webUSB.descriptor);
    }

    void attach() {
      _midi.interface.begin();
    }

    uint32_t getConnectionSequence();
    void     readSerial(char* serial);

    bool connected() {
      return _device.device->ready();
    }

    bool idle() {
      return (unsigned long)(micros() - _device.usec) > 1000;
    }

    // MIDI interface
    void setPorts(uint8_t nPorts) {
      _midi.interface.setCables(nPorts);
    }

    void setPortName(uint8_t port, const char* name) {
      _midi.interface.setCableName(port, name);
    }

    bool send(uint8_t packet[4]);
    bool receive(uint8_t packet[4]);

  private:
    struct {
      // The large descriptor is needed to carry the data for more than 3 MIDI ports.
      uint8_t descriptor[1024];

      // The tinyUSB device created by the Arduino core.
      Adafruit_USBD_Device* device{&TinyUSBDevice};

      unsigned long usec{};
    } _device;

    // The MIDI Interface of the device.
    struct {
      Adafruit_USBD_MIDI interface;
    } _midi;

    // The WebUSB interface to annouce the configuration URL to the browser.
    struct {
      struct {
        uint8_t bLength{3};
        uint8_t bDescriptorType{3}; // WEBUSB_URL
        uint8_t bScheme{1};         // https://
        char    url[128]{};
      } descriptor;

      Adafruit_USBD_WebUSB interface;
    } _webUSB;
  };
};
