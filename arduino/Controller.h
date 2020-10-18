#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <SPI.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "VirtualDevice.h"
#include "Secret.h"

#define MAX_DEVICES 10

int status = WL_IDLE_STATUS;
WiFiClient client;

class Controller {
private:
  VirtualDevice* _devices[MAX_DEVICES];
  int _deviceIndex = 0;

public:
  PubSubClient* mqtt;
  
  bool addDevice(VirtualDevice* device) {
    if (_deviceIndex >= MAX_DEVICES) {
      Serial.println("Device limit reached");
      return false;
    }
    device->setMqtt(mqtt);
    _devices[_deviceIndex] = device;
    _deviceIndex++;
    return true;
  }

  void handleMessage(char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';
    String s = String((char*)payload);
    Serial.print("Recieved message: ");
    Serial.print(topic);
    Serial.print(" - ");
    Serial.println(s);
    if (strcmp(topic, "introduce") == 0) {
      Serial.println("Sending introductions...");
      for (int i = 0; i < _deviceIndex; i++) {
        Serial.print("Device ");
        Serial.print(i);
        if (_devices[i]) {
          Serial.println(" is valid.");
          _devices[i]->introduce();
        }
        else
          Serial.println(" is not valid.");
      }
    } else {
      DynamicJsonDocument data(1024);
      deserializeJson(data, s);
      for (int i = 0; i < _deviceIndex; i++) {
        if (_devices[i] && _devices[i]->isSubscribed(topic))
          _devices[i]->handleMessage(topic, data.as<JsonObject>());
      }
    }
    Serial.println("Message handled");
  }

  void connectWifi() {
    Serial.println("Connecting to WiFi...");
    while (status != WL_CONNECTED) {
      /*if (!WDT->STATUS.bit.SYNCBUSY)
        REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;*/
      status = WiFi.begin(SECRET_NET_SSID, SECRET_NET_PASSWORD);
      delay(1000);
    }

    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("signal strength (RSSI):");
    Serial.println(WiFi.RSSI());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }

  void connectMqtt() {    
    Serial.println("Connecting to MQTT...");
    mqtt->setServer(SECRET_MQTT_SERVER, SECRET_MQTT_PORT);
    if (!mqtt->setBufferSize(1024))
      Serial.println("Failed to set buffer size");
    Serial.print("Buffer size: ");
    Serial.println(mqtt->getBufferSize());
    String clientId = "arduinoClient-";
    clientId += String(random(0xffff), HEX);
    while (!mqtt->connected()) {
      /*if (!WDT->STATUS.bit.SYNCBUSY)
        REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;*/
      if (!mqtt->connect(clientId.c_str(), SECRET_OPTIONAL_MQTT_USERNAME, SECRET_OPTIONAL_MQTT_PASSWORD)) {
        delay(1000);
        Serial.println("Connection failed, retrying...");
      }
    }
  
    Serial.print("MQTT State: ");
    Serial.println(mqtt->state());
    mqtt->subscribe("introduce");
    for (int i = 0; i < _deviceIndex; i++) {
      if (_devices[i]) {
        _devices[i]->subscribe();
        _devices[i]->introduce();
      }
    }
  }

  void setup() {    
    Serial.begin(9600);
    Serial.println("Starting up...");

    if (WiFi.status() == WL_NO_MODULE) {
      Serial.println("Communication with WiFi module failed!");
      // don't continue
      while (true);
    }
  
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
      Serial.println("Please upgrade the firmware");
    }
  
    /*REG_WDT_CONFIG = WDT_CONFIG_PER_1K;
    REG_WDT_CTRL = WDT_CTRL_ENABLE;
    while(WDT->CTRL.bit.ENABLE);*/
    
    mqtt = new PubSubClient(client);
    randomSeed(micros());
    Serial.println("Setup complete");
  }

  void loop() {
    /*if (!WDT->STATUS.bit.SYNCBUSY)
      REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;*/
    status = WiFi.status();
    if (status != WL_CONNECTED)
      this->connectWifi();
    if (!mqtt->connected())
      this->connectMqtt();
    mqtt->loop(); 
    for (int i = 0; i < _deviceIndex; i++) {
      if (_devices[i])
        _devices[i]->update();
    }
  }
};
#endif
