#ifndef __GPS_H__
#define __GPS_H__

#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include "VirtualDevice.h"

class GPS : public VirtualDevice {
private:
  long _updateInterval;
  long _timer;
  TinyGPSPlus _gps;
  
public:
  GPS(String id, String name, String icon)
    : VirtualDevice(id, name, icon) {
    _updateInterval = 15000;
    _timer = 0;
    
    Serial1.begin(9600);
  }

  void update() {
    VirtualDevice::update();

    for (unsigned long start = millis(); millis() - start < 1000;) {
      while (Serial1.available())
        _gps.encode(Serial1.read());
    }

    if (millis() - _timer > _updateInterval) {
      _timer = millis();
      
      DynamicJsonDocument data(1024);
      data["latitude"] = _gps.location.lat();
      data["longitude"] = _gps.location.lng();
      data["accuracy"] = _gps.hdop.value();
      data["speed"] = _gps.speed.kmph();
      data["satellites"] = _gps.satellites.value();

      String output;
      serializeJson(data, output);
      _mqtt->publish("data/location", output.c_str());
    }
  }
};

#endif
