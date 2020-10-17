#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "VirtualDevice.h"

class Light : public VirtualDevice {
private:
  int _lightPin;
  bool _on;
  int _brightness;
  int _rotateAmount;

  int _min;
  int _max;
  
public:
  Light(int min, int max, int lightPin, String id, String name, String icon)
    : VirtualDevice(id, name, icon) {
    pinMode(lightPin, OUTPUT);
      
    _lightPin = lightPin;
    _on = false;
    _brightness = 255;
    _rotateAmount = 10;
    _min = min;
    _max = max;
  }

  void subscribe() {
    VirtualDevice::subscribe();
    subTopic("device/" + _id + "/toggle");
    subTopic("device/" + _id + "/brightness");
  }

  void getStateJson(JsonObject object) {
    object["toggle"] = _on;
    object["brightness"] = _brightness;
  }

  void getCapabilitiesJson(JsonObject object) {
    object["toggle"] = "bool";
    object["brightness"] = "percent";
  }
  
  void handleMessage(String topic, String payload) {
    if (topic == "device/" + _id + "/toggle") {
      _on = payload.toInt() == 1;
      updateState();
      setState();
    }
    if (topic == "device/" + _id + "/brightness") {
      _brightness = payload.toInt();
      _on = true;
      updateState();
      setState();
    }
  }

  void setState() {
    if (_on && _brightness > 0) {
      if (_brightness == 255)
        digitalWrite(_lightPin, HIGH);
      else
        analogWrite(_lightPin, map(_brightness, 0, 255, _min, _max));
    } else
      digitalWrite(_lightPin, LOW);
  }
};

#endif
