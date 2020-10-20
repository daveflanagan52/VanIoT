#ifndef __HEATER_H__
#define __HEATER_H__

#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "VirtualDevice.h"

enum command_enum {
  CMD_NONE = 0,
  CMD_START = 160,
  CMD_STOP = 5,
};

enum mode_enum {
  MODE_THERMO = 50,
  MODE_FIXED = 205,
};

struct request_t
{
  uint8_t deviceType;
  uint8_t dataSize;
  uint8_t command;
  uint8_t temperature;
  uint8_t setTemperature;
  uint8_t minPumpFreq;
  uint8_t maxPumpFreq;
  uint16_t minFanSpeed;
  uint16_t maxFanSpeed;
  uint8_t operatingVoltage;
  uint8_t fanSpeedSensor;
  uint8_t mode;
  uint8_t minTemperature;
  uint8_t maxTemperature;
  uint8_t glowPlugPower;
  uint8_t prime;
  uint16_t unknown1;
  uint16_t altitude;
  uint16_t crc;
};

struct response_t
{
  uint8_t deviceType;
  uint8_t dataSize;
  uint8_t runState;
  uint8_t onOff;
  uint16_t supplyVoltage;
  uint16_t fanRPM;
  uint16_t fanVoltage;
  uint16_t heatExchangerTemp;
  uint16_t glowPlugVoltage;
  uint16_t glowPlugCurrent;
  uint8_t pumpFreq;
  uint8_t errorCode;
  uint8_t unknown1;
  uint8_t fixedPupFreq;
  uint8_t unknown2;
  uint8_t unknown3;
  uint16_t crc;
};

class Heater : public VirtualDevice {
private:
  String _tempSensor;
  int _enablePin;
  
  long _lastPub;
  long _publishInterval;
  request_t* _request;
  response_t* _response;
  response_t* _prevResponse;

  bool _requestStop;
  bool _requestStart;
  byte _requestTemp;
  byte _ambientTemp;
  
public:
  Heater(String tempSensor, int enablePin, String id, String name, String icon)
    : VirtualDevice(id, name, icon) {
    _tempSensor = tempSensor;
    _enablePin = enablePin;
    _requestTemp = 25;

    _request = new request_t();
    _request->deviceType = 118;
    _request->dataSize = 22;
    _request->minPumpFreq = 14;
    _request->maxPumpFreq = 30;
    _request->minFanSpeed = 1500;
    _request->maxFanSpeed = 3200;
    _request->operatingVoltage = 120;
    _request->fanSpeedSensor = 1;
    _request->mode = MODE_THERMO;
    _request->minTemperature = 8;
    _request->maxTemperature = 35;
    _request->glowPlugPower = 5;
    _request->unknown = 60231;
    _request->altitude = 3500;

    _response = new response_t();
    _prevResponse = new response_t();
    
    _publishInterval = 500;
    _lastPub = 0;
    pinMode(_enablePin, OUTPUT);
    Serial1.begin(25000);
  }

  void subscribe() {
    VirtualDevice::subscribe();
    subTopic("device/" + _id + "/toggle");
    subTopic("device/" + _id + "/temperature");
  }

  void getStateJson(JsonObject object) {
    object["toggle"] = _response->onOff;
    object["temperature"] = _requestTemp;
    object["state"] = _response->runState;
    object["pumpFreq"] = _response->pumpFreq;
    object["errorCode"] = _response->errorCode;
  }

  void getCapabilitiesJson(JsonObject object) {
    object["toggle"] = "bool";
    object["temperature"] = "number:0:35";
  }
  
  void handleMessage(String topic, JsonObject payload) {
    if (topic == "device/" + _id + "/toggle") {
      _requestStart = payload["value"] == 1;
      _requestStop = payload["value"] != 1;
    }
    if (topic == "device/" + _id + "/temperature") {
      _requestTemp = payload["value"];
    }
    if (topic == "device/" + _tempSensor + "/state") {
      _ambientTemp = payload["value"];
    }
  }

  unsigned int computeCRC(byte* buf, int len) {
    unsigned int crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
      crc ^= (unsigned int)buf[pos];
    
      for (int i = 8; i != 0; i--) {
        if ((crc & 0x0001) != 0) {
          crc >>= 1;
          crc ^= 0xA001;
        }
        else
          crc >>= 1;
      }
    }
    return crc;  
  }

  void update() {
    VirtualDevice::update();
    
    long ms = millis();
    if (_lastPub + _publishInterval < ms) {
      _lastPub = ms;

      if (_requestStop) { // Turn off
        _request->command = CMD_STOP;
        _requestStop = false;
      }
      else if(_requestStart) { // Turn on
        _request->command = CMD_START;
        _requestStart = false;
      }
      else
        _request->command = CMD_NONE;
      _request->temperature = _ambientTemp;
      _request->setTemperature = _requestTemp;

      unsigned int crc = computeCRC((byte*)_request, 22);
      _request->crc = word(bitRead(crc, 0), bitRead(crc, 1));
      
      digitalWrite(_enablePin, HIGH);
      Serial1.write((byte*)&_request, sizeof(_request));
      digitalWrite(_enablePin, LOW);
  
      Serial1.readBytes((byte*)_response, sizeof(response_t));
      bool changed = false;
      crc = computeCRC((byte*)_response, 22);
      if (_response->crc != word(bitRead(crc, 0), bitRead(crc, 1)))
        Serial.println("Invalid response CRC, data may be invalid");      
      if (_response->runState != _prevResponse->runState)
        changed = true;
      if (_response->onOff != _prevResponse->onOff)
        changed = true;      
      if (_response->pumpFreq != _prevResponse->pumpFreq)
        changed = true;      
      if (_response->errorCode != _prevResponse->errorCode)
        changed = true;
      if (changed)
        updateState();   
      memcpy(&_prevResponse, &_response, sizeof(response_t));   
    }
  }
};

#endif
