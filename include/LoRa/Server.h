#ifndef LORASERVER_H
#define LORASERVER_H

#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <WiFi.h>

// info for system info.
#include "freertos/task.h"
#include <esp_system.h>
#include <esp_wifi.h>
#include <Arduino.h>
#include "LoRa.h"
#include <SPI.h>

// importing airsensor functions.
#include "AirSensor/AirSensor.h"
#include "LoRa/LoRa.h"

void createAccessPoint();
void connectToWifi();
void serverRoutes();
void defaultServices();
String getWifiScanJson();
String generateSystemInfoJson();

// Definición de la estructura Route
struct Route {
  const char* uri;
  const char* path;
  const char* contentType;
};
// Declaración del arreglo de rutas
extern Route routes[];

#endif