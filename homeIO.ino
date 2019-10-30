#include <WiFi.h>
extern "C" {
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
}
#include <AsyncMqttClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "config.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Bootando");
  
  setupMQTT();
  setupOTA();
  setupIO();
}

void loop() {
  loopOTA();
  loopIO();
  
}
