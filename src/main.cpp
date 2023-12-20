/*
        _          _ ______ _____ _____
       (_)        | |  ____|  __ \_   _|
  _ __  ___  _____| | |__  | |  | || |
 | '_ \| \ \/ / _ \ |  __| | |  | || |
 | |_) | |>  <  __/ | |____| |__| || |_
 | .__/|_/_/\_\___|_|______|_____/_____|
 | |
 |_|

www.pixeledi.eu | https://linktr.ee/pixeledi
E-INK Avalanche Report on e-ink | V1.0 | 08.2023

  # Data Flow:
  * WIFI and MQTT Connection will be established
  * MQTT send to Node-RED 
  * HTTP-Request in Node-RED 
  * Node-RED parses JSON
  * One specific Region is fetched
  * JSON with avalanche data is sent via MQTT to ESP32
  * Show data on E-INK

*/

#include <Arduino.h>
#include <WiFi.h>
#include "globals.h"
#include "eink.h"
#include "credentials.h"
#include <PubSubClient.h>
#include "wifimqtt.h"
#include <ArduinoJson.h>

void setDeepSleep()
{
  Serial.println("Going to sleep zzZZZzz");
  // ESP32 XIAO spezifisch
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  // DEBUG
  //ESP.deepSleep(20 * 1e6);
  ESP.deepSleep(59 * 60 * 1e6);
}

void setup()
{
  Serial.begin(115200);
  display.init();
  connectAP();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  // mqtt packet size
  client.setBufferSize(1024);
  state = GETAVALANCHEREPORT;
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  if (!client.loop())
  {
    client.connect("ESP32Avalanche");
  }

  unsigned long currentMillis = millis();

  switch (state)
  {
  case GETAVALANCHEREPORT:
    if (currentMillis - previousMillis >= (1000 * 3) && startup)
    {
      previousMillis = currentMillis;
      Serial.println("Trigger HTTP-Request in Node-RED");
      client.publish("getAvalancheReport", "get new avalanche report");
      startup = false;
    }
    break;

  case REFRESHEINK:
     Serial.println("state refresheink");
    showAvalancheDataOnDisplay();
    break;

  case GOINGSLEEP:
     
    if (currentMillis - previousMillis >= (1000 * 10) && !startup)
    {
      previousMillis = currentMillis;
       Serial.println("state sleep");
      setDeepSleep();
    }
    break;
  case WAIT:
  {
    previousMillis = currentMillis;
    Serial.println("state wait");
    state = GOINGSLEEP;
    break;
  }
  }
}