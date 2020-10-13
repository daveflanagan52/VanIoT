#ifndef __FAN_H__
#define __FAN_H__

#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "VirtualDevice.h"

class Fan : public VirtualDevice {
private:
  bool _on;
  bool _open;
  int _direction;
  int _speed;
  
public:
  Fan(String id, String name, String icon)
    : VirtualDevice(id, name, icon) {
    _on = false;
    _open = false;
    _direction = 1;
    _speed = 0;
  }

  void subscribe() {
    VirtualDevice::subscribe();
    subTopic("device/" + _id + "/toggle");
    subTopic("device/" + _id + "/open");
    subTopic("device/" + _id + "/direction");
    subTopic("device/" + _id + "/speed");
  }

  void getStateJson(JsonObject object) {
    object["toggle"] = _on;
    object["open"] = _open;
    object["direction"] = _direction;
    object["speed"] = _speed;
  }

  void getCapabilitiesJson(JsonObject object) {
    object["toggle"] = "bool";
    object["open"] = "bool";
    object["direction"] = "number";
    object["speed"] = "percent";
  }
  
  void handleMessage(String topic, String payload) {
    if (topic == "device/" + _id + "/toggle") {
      _on = payload.toInt() == 1;
      updateState();
    }
    if (topic == "device/" + _id + "/open") {
      _open = payload.toInt() == 1;
      updateState();
    }
    if (topic == "device/" + _id + "/direction") {
      _direction = payload.toInt();
      updateState();
    }
    if (topic == "device/" + _id + "/speed") {
      _speed = payload.toInt();
      updateState();
    }
  }
};

#endif
