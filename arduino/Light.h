#ifndef __LIGHT_H__
#define __LIGHT_H__

#define DISABLE_INPUT true

#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "VirtualDevice.h"

class Light : public VirtualDevice {
private:
  int _lightPin;
  int _buttonPin;
  int _encoderPinA;
  int _encoderPinB;
  bool _on;
  int _brightness;
  int _rotateAmount;

  int _min;
  int _max;

  int _lastButtonState;
  int _lastEncoderAState;
  
public:
  Light(int min, int max, int lightPin, int buttonPin, int encoderPinA, int encoderPinB, String id, String name, String icon)
    : VirtualDevice(id, name, icon) {
    pinMode(lightPin, OUTPUT);
    pinMode(buttonPin, INPUT);
    pinMode(encoderPinA, INPUT);
    pinMode(encoderPinB, INPUT);
    digitalWrite(encoderPinA, HIGH);
    digitalWrite(encoderPinB, HIGH);
      
    _lightPin = lightPin;
    _buttonPin = buttonPin;
    _encoderPinA = encoderPinA;
    _encoderPinB = encoderPinB;
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
    }
    if (topic == "device/" + _id + "/brightness") {
      _brightness = payload.toInt();
      _on = true;
      updateState();
    }
  }

  void update() {
    VirtualDevice::update();

#ifndef DISABLE_INPUT
    int encoderAState = digitalRead(_encoderPinA);
    if (encoderAState != _lastEncoderAState) {
      if (digitalRead(_encoderPinB))
        _brightness += _rotateAmount;
      else
        _brightness -= _rotateAmount;
      _brightness = constrain(_brightness, 0, 255);
      updateState();
      _lastEncoderAState = encoderAState;
    }
      
    int button = digitalRead(_buttonPin);
    if (button != _lastButtonState) {
      if (button == HIGH) {
        _on = !_on;
        updateState();
      }
      _lastButtonState = button;
    }
#endif
    
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
