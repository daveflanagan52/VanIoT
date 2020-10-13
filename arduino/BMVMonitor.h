#ifndef __BMV_MONITOR_H__
#define __BMV_MONITOR_H__

#include <VEDirect.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "wiring_private.h"
#include "VirtualDevice.h"

Uart bmvSerial (&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);

void SERCOM0_Handler() {
  bmvSerial.IrqHandler();
}

class BMVMonitor : public VirtualDevice {
private:
  VEDirect* _bmv;
  long _lastPub;
  long _publishInterval;
  
public:
  BMVMonitor(String id, String name, String icon)
    : VirtualDevice(id, name, icon) {    
    pinPeripheral(5, PIO_SERCOM_ALT);
    pinPeripheral(6, PIO_SERCOM_ALT);    
    _bmv = new VEDirect(bmvSerial);
    _publishInterval = 5000;
    _lastPub = 0;
  }

  void update() {
    VirtualDevice::update();
    
    long ms = millis();
    if (_lastPub + _publishInterval < ms) {
      _lastPub = ms;
      publish();
    }
  }

  void publish() {
    DynamicJsonDocument data(1024);
    data["soc"] = _bmv->read(VE_SOC);
    data["voltage"] = _bmv->read(VE_VOLTAGE);
    data["current"] = _bmv->read(VE_CURRENT);
    data["power"] = _bmv->read(VE_POWER);

    String output;
    serializeJson(data, output);
    _mqtt->publish("data/battery", output.c_str());
  }
};

#endif
