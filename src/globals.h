#include <Arduino.h>
#ifndef GLOBALS_
#define GLOBALS_

unsigned long previousMillis = millis();

extern void showAvalancheDataOnDisplay();

bool startup=true;
String jsonPayload="";

enum states {
  GETAVALANCHEREPORT,
  REFRESHEINK,
  GOINGSLEEP,
  WAIT
} state;

struct avalancheSingleData{
  String region;
  String avproblem0;
  String avproblem1;
  String dangerpattern0;
  String dangerpattern1;
  String publicationTime;
  String activityhighlights;
  String tendencyType;
  String avalancheSize;
};
avalancheSingleData avalancheData;

#endif