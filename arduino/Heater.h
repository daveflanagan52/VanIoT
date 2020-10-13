#ifndef __HEATER_H__
#define __HEATER_H__

#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "VirtualDevice.h"

class Heater : public VirtualDevice {
private:
  bool _on;
  int _temperature;
  
public:
  Heater(String id, String name, String icon)
    : VirtualDevice(id, name, icon) {
    _on = false;
    _temperature = 23;
  }

  void subscribe() {
    VirtualDevice::subscribe();
    subTopic("device/" + _id + "/toggle");
    subTopic("device/" + _id + "/temperature");
  }

  void getStateJson(JsonObject object) {
    object["toggle"] = _on;
    object["temperature"] = _temperature;
  }

  void getCapabilitiesJson(JsonObject object) {
    object["toggle"] = "bool";
    object["temperature"] = "number";
  }
  
  void handleMessage(String topic, String payload) {
    if (topic == "device/" + _id + "/toggle") {
      _on = payload.toInt() == 1;
      updateState();
    }
    if (topic == "device/" + _id + "/temperature") {
      _temperature = payload.toInt();
      updateState();
    }
  }
};

#endif
