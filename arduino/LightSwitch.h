#ifndef __LIGHT_SWITCH_H__
#define __LIGHT_SWITCH_H__

#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "VirtualDevice.h"

class LightSwitch : public VirtualDevice {
private:
  int _buttonPin;
  int _encoderPinA;
  int _encoderPinB;
  bool _on;
  int _brightness;
  int _rotateAmount;
  String _light;

  int _lastButtonState;
  int _lastEncoderAState;
  
public:
  LightSwitch(int buttonPin, int encoderPinA, int encoderPinB, String light, String id, String name, String icon)
    : VirtualDevice(id, name, icon) {
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(encoderPinA, INPUT);
    pinMode(encoderPinB, INPUT);
    digitalWrite(encoderPinA, HIGH);
    digitalWrite(encoderPinB, HIGH);
      
    _buttonPin = buttonPin;
    _encoderPinA = encoderPinA;
    _encoderPinB = encoderPinB;
    _light = light;
    _rotateAmount = 10;

    _on = false;
    _brightness = 255;
  }

  void subscribe() {
    VirtualDevice::subscribe();
    subTopic("device/" + _light + "/state");
  }

  void handleMessage(String topic, JsonObject payload) {
    if (topic == "device/" + _light + "/state") {
      _on = payload["toggle"];
      _brightness = payload["brightness"];
    }
  }

  void update() {
    VirtualDevice::update();

    int encoderAState = digitalRead(_encoderPinA);
    if (encoderAState != _lastEncoderAState) {
      if (digitalRead(_encoderPinB))
        _brightness += _rotateAmount;
      else
        _brightness -= _rotateAmount;
      _brightness = constrain(_brightness, 0, 255);
      
      // Send brightness

      _lastEncoderAState = encoderAState;
    }
      
    int button = digitalRead(_buttonPin);
    if (button == LOW && _lastButtonState == HIGH) {
      _on = !_on;
      // Send state
      _lastButtonState = button;
    }
  }
};

#endif
