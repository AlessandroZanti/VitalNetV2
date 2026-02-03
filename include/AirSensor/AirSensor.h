#ifndef AIRSENSOR_H
#define AIRSENSOR_H

#include <ESPAsyncWebServer.h>
#include <ScioSense_ENS160.h>
#include <Adafruit_AHTX0.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <Wire.h>

// Declaraciones de las funciones
String handleAirRequest();
void airSensorBegin();
void getTemp();
void showTemp();

#endif