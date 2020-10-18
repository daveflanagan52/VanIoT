#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

// Pull up Resistor sizing
// Length  5V    3V
// 10cm    10K   6,8K
// 20cm    8,2K  4,7K
// 50cm    4,7K  3,3K
// 100cm   3,3K  2,2K
// 200cm   2,2K  1K
// 500cm   1K    -

#include <VEDirect.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "wiring_private.h"
#include "VirtualDevice.h"
#include <OneWire.h>
#include <DS18B20.h>

class Temerature : public VirtualDevice {
private:
  long _lastPub;
  long _publishInterval;
  int _pin;
  String _location;  
  OneWire* _oneWire;
  DS18B20* _sensor;
  
public:
  Temerature(int pin, String location, String id, String name, String icon)
    : VirtualDevice(id, name, icon) {
    _publishInterval = 5000;
    _lastPub = 0;
    _location = location;
    _oneWire = new OneWire(pin);
    _sensor = new DS18B20(_oneWire);
    _sensor->begin();
  }

  void update() {
    VirtualDevice::update();
    
    long ms = millis();
    if (_lastPub + _publishInterval < ms) {
      _lastPub = ms;

      _sensor->requestTemperatures();
      while (!_sensor->isConversionComplete());
      
      publish();
    }
  }

  void publish() {
    DynamicJsonDocument data(1024);
    data["value"] = _sensor->getTempC();

    String output;
    serializeJson(data, output);
    _mqtt->publish(("data/temperature/" + _location).c_str(), output.c_str());
  }
};

#endif
