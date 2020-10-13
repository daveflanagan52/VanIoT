#include "Devices.h"
#include "Controller.h"

#define DEV 2
Controller controller;

void handleMessage(char* topic, byte* payload, unsigned int length) {
  controller.handleMessage(topic, payload, length);
}

void setup() {
  controller.setup();
  controller.mqtt->setCallback(handleMessage);

#if DEV == 1
  controller.addDevice(new BMVMonitor("18af89ec-587c-4f14-b339-59e328f8f444", "Battery Stats", "car-battery"));
  controller.addDevice(new Toggle(9, "c327d2d6-d8f1-4f57-94b3-08971eefd474", "Invertor", "outlet"));
#endif

#if DEV == 2
  controller.addDevice(new Light(85, 110, A0, 2, 3, 4, "7b40d06b-bef3-496a-9fa9-08f82078be78", "Front Lights", "lightbulb"));
  controller.addDevice(new Light(250, 255, A1, 5, 6, 8, "c5894f6c-0bf5-4cb2-9987-f39a0b786d38", "Kitchen Lights", "lightbulb"));
  controller.addDevice(new Light(50, 150, A2, 9, 10, 11, "2cbfc82e-2541-4777-9a91-93b052842f0a", "Rear Lights", "lightbulb"));
  controller.addDevice(new Toggle(A3, "88b3a6a8-ae20-43b7-bdf3-ba70a83db81a", "Cooling Fan", "fan"));
#endif

#if DEV == 3
  controller.addDevice(new Heater("63e2f136-380f-4f69-aa02-c06cf987688d", "Heater", "heat"));
#endif

#if DEV == 4
  controller.addDevice(new Fan("b6932790-e9e0-4fe8-9ffb-b8a4a4a3f740", "Heater", "fan"));
#endif

#if DEV == 5
  controller.addDevice(new GPS("96c9f1f0-2386-4a1e-8822-a208090fb911", "GPS Tracker", "location"));
#endif
}

void loop() {
  controller.loop();
}
