#ifndef __GAS_H__
#define __GAS_H__

#include <VEDirect.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "wiring_private.h"
#include "VirtualDevice.h"

class Gas : public VirtualDevice {
private:
  long _lastPub;
  long _publishInterval;
  int _sensorPin;
  int _heaterPin;
  int _rawValue;
  double _c1;
  double _c2;
  String _type;
  
public:
  Gas(int sensorPin, int heaterPin, double c1, double c2, String type, String id, String name, String icon)
    : VirtualDevice(id, name, icon) {
    _publishInterval = 5000;
    _lastPub = 0;
    _sensorPin = sensorPin;
    _heaterPin = heaterPin;
    _c1 = c1;
    _c2 = c2;
    _type = type;
    pinMode(_sensorPin, OUTPUT);
    pinMode(_heaterPin, OUTPUT);
  }

  void update() {
    VirtualDevice::update();

    _rawValue = analogRead(_sensorPin);
    
    long ms = millis();
    if (_lastPub + _publishInterval < ms) {
      _lastPub = ms;
      publish();
    }
  }

  double convertRaw() {
    return _c1 * exp(_c2 * (_rawValue*3.3/4095));
  }

  void publish() {
    DynamicJsonDocument data(1024);
    data["ppm"] = convertRaw();

    String output;
    serializeJson(data, output);
    _mqtt->publish(("data/gas/" + _type).c_str(), output.c_str());
  }
};

#endif
