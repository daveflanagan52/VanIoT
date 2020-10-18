#ifndef __HEATER_H__
#define __HEATER_H__

#include <PubSubClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "VirtualDevice.h"

class Heater : public VirtualDevice {
private:
  String _tempSensor;
  int _enablePin;
  
  long _lastPub;
  long _publishInterval;
  byte* _request;
  byte* _response;

  bool _requestStop;
  bool _requestStart;
  byte _requestTemp;
  byte _ambientTemp;

  byte _runState;
  byte _on;
  int _supplyVoltage;
  int _fanRPM;
  int _fanVoltage;
  int _heatExchangerTemp;
  int _glowPlugVoltage;
  int _glowPlugCurrent;
  byte _pumpFreq;
  byte _errorCode;
  
public:
  Heater(String tempSensor, int enablePin, String id, String name, String icon)
    : VirtualDevice(id, name, icon) {
    _tempSensor = tempSensor;
    _enablePin = enablePin;
    _on = false;
    _requestTemp = 25;

    _request = new byte[24];
    _request[0] = 0x76;
    _request[1] = 0x16;
    _request[2] = 0x00;  // Command, 0x05 = stop, 0xA0 = start
    _request[3] = 0x00;  // Temperature
    _request[4] = 0x00;  // Set temperature
    _request[5] = 0x0E;  // Minimum Pump frequency
    _request[6] = 0x1E;  // Maximum Pump frequency
    _request[7] = bitRead(0x05DC, 1);  // Minimum fan speed msb
    _request[8] = bitRead(0x05DC, 0);  // Minimum fan speed lsb
    _request[9] = bitRead(0x0C80, 1);  // Maximum fan speed msb
    _request[10] = bitRead(0x0C80, 0); // Maximum fan speed lsb
    _request[11] = 0x78; // Operating voltage
    _request[12] = 0x01; // Fan speed sensor
    _request[13] = 0x32; // Mode, 0xCD fixed mode
    _request[14] = 0x08; // Min temperature
    _request[15] = 0x23; // Max temperature
    _request[16] = 0x05; // Glow plug power
    _request[17] = 0x00; // Prime 0x5A
    _request[18] = 0xEB; // ???
    _request[19] = 0x47; // ???
    _request[20] = bitRead(0x0DAC, 1); // Altitude msb
    _request[21] = bitRead(0x0DAC, 1); // Altitude lsb
    _request[22] = 0x00; // CRC-16/MODBUS msb
    _request[23] = 0x00; // CRC-16/MODBUS lsb
    
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
    object["toggle"] = _on;
    object["temperature"] = _requestTemp;
    object["state"] = _runState;
    //object["supplyVoltage"] = _supplyVoltage;
    //object["fanRPM"] = _fanRPM;
    //object["fanVoltage"] = _fanVoltage;
    //object["heatExchangerTemp"] = _heatExchangerTemp;
    //object["glowPlugVoltage"] = _glowPlugVoltage;
    //object["glowPlugCurrent"] = _glowPlugCurrent;
    object["pumpFreq"] = _pumpFreq;
    object["errorCode"] = _errorCode;
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
        _request[2] = 0x05;
        _requestStop = false;
      }
      else if(_requestStart) { // Turn on
        _request[2] = 0xA0;
        _requestStart = false;
      }
      else
        _request[2] = 0x00;
      _request[3] = _ambientTemp;
      _request[4] = _requestTemp;

      unsigned int crc = computeCRC(_request, 22);
      _request[22] = bitRead(crc, 0);
      _request[23] = bitRead(crc, 1);
      
      digitalWrite(_enablePin, HIGH);
      for (int i = 0; i < 24; i++)
        Serial1.print(_request[i]);
      digitalWrite(_enablePin, LOW);
  
      int i = 0;
      while (Serial1.available()) {
        _response[i] = Serial1.read();
        Serial.print(_response[i], HEX);
        i++;
      }
      Serial.println();

      bool changed = false;

      crc = computeCRC(_response, 22);
      if (word(_response[23], _response[22]) != crc)
        Serial.println("Invalid response CRC, data may be invalid");
      
      if (_response[2] != _runState) {
        _runState = _response[2];
        changed = true;
      }
      
      if (_response[3] != _on) {
        _on = _response[3];
        changed = true;
      }

      int supplyVoltage = word(_response[4], _response[5]);
      if (supplyVoltage != _supplyVoltage) {
        _supplyVoltage = supplyVoltage;
        //changed = true;
      }

      int fanRPM = word(_response[6], _response[7]);
      if (fanRPM != _fanRPM) {
        _fanRPM = fanRPM;
        //changed = true;
      }

      int fanVoltage = word(_response[8], _response[9]);
      if (fanVoltage != _fanVoltage) {
        _fanVoltage = fanVoltage;
        //changed = true;
      }

      int heatExchangerTemp = word(_response[10], _response[11]);
      if (heatExchangerTemp != _heatExchangerTemp) {
        _heatExchangerTemp = heatExchangerTemp;
        //changed = true;
      }

      int glowPlugVoltage = word(_response[12], _response[13]);
      if (glowPlugVoltage != _glowPlugVoltage) {
        _glowPlugVoltage = glowPlugVoltage;
        //changed = true;
      }

      int glowPlugCurrent = word(_response[14], _response[15]);
      if (glowPlugCurrent != _glowPlugCurrent) {
        _glowPlugCurrent = glowPlugCurrent;
        //changed = true;
      }
      
      if (_response[16] != _pumpFreq) {
        _pumpFreq = _response[16];
        changed = true;
      }
      
      if (_response[17] != _errorCode) {
        _errorCode = _response[17];
        changed = true;
      }

      if (changed)
        updateState();
      
    }
  }
};

#endif
