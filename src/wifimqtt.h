#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "globals.h"

// MQTT
String clientID = "ESP32-";
const char *mqtt_server = "167.235.226.158";
const char *mqtt_user = "pixeledi";
const char *mqtt_password = "noderedgadgets#1";
WiFiClient espClient;
PubSubClient client(espClient);

extern void setDeepSleep();

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    clientID += String(random(0xffff), HEX);
    if (client.connect(clientID.c_str(), mqtt_user, mqtt_password))
    {
      Serial.println("connect to MQTT");
      client.subscribe("avalancheReport");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char *topic, byte *message, unsigned int length)
{
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    messageTemp += (char)message[i];
  }

  if (String(topic) == "avalancheReport")
  {
    Serial.println(messageTemp);
    // Stream& input;

    StaticJsonDocument<2048> doc;

    DeserializationError error = deserializeJson(doc, messageTemp);

    if (error)
    {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    const char *region = doc["region"];                         // "Nördliche Zillertaler Alpen"
    const char *avProblem0 = doc["avProblem0"];                 // "persistent_weak_layers"
    const char *avProblem1 = doc["avProblem1"];                 // "gliding_snow"
    const char *dangerPattern0 = doc["dangerPattern0"];         // "DP6"
    const char *dangerPattern1 = doc["dangerPattern1"];         // "DP4"
    const char *publicationTime = doc["publicationTime"];       // "01.12.23 07:00"
    const char *activityhighlights = doc["activityhighlights"]; // "Mit Neuschnee und Wind steigt die Gefahr ...
    const char *tendencyType = doc["tendencyType"];             // "increasing"
    int avalancheSize = doc["avalancheSize"];                   // 3

    avalancheData.region = String(region);
    avalancheData.avproblem0 = String(avProblem0);
    avalancheData.avproblem1 = String(avProblem1);
    avalancheData.dangerpattern0 = String(dangerPattern0);
    avalancheData.dangerpattern1 = String(dangerPattern1);
    avalancheData.publicationTime = String(publicationTime);
    avalancheData.activityhighlights = String(activityhighlights);
    avalancheData.tendencyType = String(tendencyType);
    avalancheData.avalancheSize = String(avalancheSize);

    // update eink
    state=REFRESHEINK;
  }
}

void connectAP()
{

  Serial.println("Connecting to WiFi..");
  WiFi.begin(ssid, password);

  int cnt = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
    cnt++;

    if (cnt > 30)
      // ESP.restart();
      setDeepSleep();
  }
  Serial.println(WiFi.localIP());
}