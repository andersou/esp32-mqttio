#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "config.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Bootando");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  setupMQTT();
  setupOTA();
  setupIO();
}

void loop() {
  loopOTA();
  loopIO();
  loopMQTT();
}
