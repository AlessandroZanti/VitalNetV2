#include "AirSensor/AirSensor.h"

Adafruit_AHTX0 aht;
ScioSense_ENS160 ens160(ENS160_I2CADDR_1);

int humidity;
int tempC;
int tempF;
int tvoc;
int eco2;
int aqi;

String handleAirRequest() {
  // Crear un objeto JSON
  DynamicJsonDocument doc(200);
  
  doc["humidity"] = humidity;
  doc["tempC"] = tempC;
  doc["tempF"] = tempF;
  doc["tvoc"] = tvoc;
  doc["eco2"] = eco2;
  doc["aqi"] = aqi;
  
  // Serializar el objeto JSON en una cadena
  String response;
  serializeJson(doc, response);
  
  return response;
}

void airSensorBegin() {
  Serial.println("------------------------------------------------------------");
  Serial.println("ENS160 - Digital air quality sensor");
  Serial.println();
  Serial.println("Sensor readout in standard mode");
  Serial.println();
  Serial.println("------------------------------------------------------------");

  delay(1000);

  Serial.print("ENS160...");
  ens160.begin();
  Serial.println(ens160.available() ? "done." : "failed!");
  if (ens160.available()) {
    Serial.print("\tRev: "); Serial.print(ens160.getMajorRev());
    Serial.print("."); Serial.print(ens160.getMinorRev());
    Serial.print("."); Serial.println(ens160.getBuild());
    Serial.print("\tStandard mode ");
    Serial.println(ens160.setMode(ENS160_OPMODE_STD) ? "done." : "failed!");
  }

  Serial.println("Adafruit AHT10/AHT20 demo!");
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
  }
  Serial.println("AHT10 or AHT20 found");
}

void getTemp() {

  sensors_event_t humidity1, temp;
  aht.getEvent(&humidity1, &temp);
  
  tempC = (temp.temperature);
  tempF = (temp.temperature)*1.8+32;
  humidity = (humidity1.relative_humidity);

  if (ens160.available()) {

    ens160.set_envdata(tempC, humidity);
    ens160.measure(true);
    ens160.measureRaw(true);

    aqi = ens160.getAQI();
    tvoc = ens160.getTVOC();
    eco2 = ens160.geteCO2();
  }
  yield();
}

void showTemp(){
  Serial.print("Temp: "); Serial.print(tempC); Serial.println("°C\t");
  Serial.print("Humidity: "); Serial.println(humidity);
}
