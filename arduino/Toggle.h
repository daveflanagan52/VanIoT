#ifndef __TOGGLE_H__
#define __TOGGLE_H__

#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "VirtualDevice.h"

class Toggle : public VirtualDevice {
private:
  int _pin;
  bool _on;
  
public:
  Toggle(int pin, String id, String name, String icon)
    : VirtualDevice(id, name, icon) {
    pinMode(pin, OUTPUT);
    _pin = pin;
    _on = false;
  }

  void subscribe() {
    VirtualDevice::subscribe();
    subTopic("device/" + _id + "/toggle");
  }

  void getStateJson(JsonObject object) {
    object["toggle"] = _on;
  }

  void getCapabilitiesJson(JsonObject object) {
    object["toggle"] = "bool";
  }
  
  void handleMessage(String topic, String payload) {
    if (topic == "device/" + _id + "/toggle") {
      _on = payload.toInt() == 1;
      digitalWrite(_pin, _on ? HIGH : LOW);
      updateState();
    }
  }
};
#endif
