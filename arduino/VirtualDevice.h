#ifndef __VIRTUAL_DEVICE_H__
#define __VIRTUAL_DEVICE_H__

#include <PubSubClient.h>
#include <ArduinoJson.h>

#define MAX_TOPICS 6
#define BUFFER_SIZE 128

class VirtualDevice {
protected:
  PubSubClient* _mqtt;
  String _id;
  String _name;
  String _icon;
  String _topics[MAX_TOPICS];
  int _topicIndex;

  void subTopic(String topic) {
    if (_topicIndex >= MAX_TOPICS) {
      Serial.print("Max topics reached for device ");
      Serial.println(_name);
      return;
    }
    _topics[_topicIndex] = topic;
    _mqtt->subscribe(topic.c_str());
    _topicIndex++;
  }

  virtual void getStateJson(JsonObject object) {}
  virtual void getCapabilitiesJson(JsonObject object) {}

public:
  VirtualDevice() {}
  VirtualDevice(String id, String name, String icon) {
    _id = id;
    _name = name;
    _icon = icon;
    _topicIndex = 0;
  }

  void setMqtt(PubSubClient* mqtt) {
    _mqtt = mqtt;
  }

  void introduce() {
    DynamicJsonDocument data(1024);
    data["id"] = _id.c_str();
    data["name"] = _name.c_str();
    data["icon"] = _icon.c_str();
    JsonObject state = data.createNestedObject("state");
    JsonObject capabilities = data.createNestedObject("capabilities");
    getStateJson(state);
    getCapabilitiesJson(capabilities);
    
    String output;
    serializeJson(data, output);
    Serial.print("Sending message: ");
    Serial.println(output.c_str());
    if (!_mqtt->publish("device", output.c_str()))
      Serial.println("Failed to send message");
  }

  void updateState() {
    DynamicJsonDocument data(1024);
    data["id"] = _id.c_str();
    JsonObject state = data.createNestedObject("state");
    getStateJson(state);
    
    String output;
    serializeJson(data, output);
    if (!_mqtt->publish("device/state", output.c_str()))
      Serial.println("Failed to send message");
  }

  virtual void handleMessage(String topic, String payload) {}
  virtual void subscribe() {
    _topicIndex = 0;
  }
  virtual void update() {}
  
  bool isSubscribed(String topic) {
    for (int i = 0; i < MAX_TOPICS; i++)
      if (topic == _topics[i])
        return true;
    return false;
  }
};

#endif
