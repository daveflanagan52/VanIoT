#include "Devices.h"
#include "Controller.h"

#define DEV 8
Controller controller;

void handleMessage(char* topic, byte* payload, unsigned int length) {
  controller.handleMessage(topic, payload, length);
}

void setup() {
  controller.setup();
  controller.mqtt->setCallback(handleMessage);

#if DEV == 1
  controller.addDevice(new BMVMonitor("battery", "Battery Stats", "car-battery"));
  controller.addDevice(new Toggle(9, "invertor", "Invertor", "outlet"));
#endif

#if DEV == 2
  controller.addDevice(new Light(85, 110, A0, "front-lights", "Front Lights", "lightbulb"));
  controller.addDevice(new Light(250, 255, A1, "kitchen-lights", "Kitchen Lights", "lightbulb"));
  controller.addDevice(new Light(50, 150, A2, "rear-lights", "Rear Lights", "lightbulb"));
#endif

#if DEV == 3
  controller.addDevice(new Heater("heater", "Heater", "heat"));
#endif

#if DEV == 4
  controller.addDevice(new Fan("fan", "Fan", "fan"));
#endif

#if DEV == 5
  controller.addDevice(new GPS("gps", "GPS Tracker", "location"));
#endif

#if DEV == 6
  controller.addDevice(new Gas(A0, 0, 26.572, 1.2894, "lpg", "lpg", "LPG Sensor", "burn"));
#endif

#if DEV == 7
  controller.addDevice(new Temperature(2, "inside", "inside-temperature", "Inside Temperature", "temperature-low"));
  controller.addDevice(new Temperature(3, "outside", "outside-temperature", "Outside Temperature", "temperature-low"));
#endif

#if DEV == 8
  controller.addDevice(new LightSwitch(2, 3, 4, "front-lights", "switch-1", "Switch 1", "light-switch"));
  controller.addDevice(new LightSwitch(5, 6, 7, "kitchen-lights", "switch-2", "Switch 2", "light-switch"));
  controller.addDevice(new LightSwitch(8, 9, 10, "rear-lights", "switch-3", "Switch 3", "light-switch"));
#endif
}

void loop() {
  controller.loop();
}
