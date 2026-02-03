#ifndef LORAFUNCTIONS_H
#define LORAFUNCTIONS_H

// Bibliotecas para LoRa
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>


#define BRILLO 250 
#define ANCHO 64
#define ALTO 8
#define PIN 23

#define POSITION 0
#define GREEN 255
#define BLUE 255
#define RED 255

#define BKGREEN 0
#define BKBLUE 0
#define BKRED 0

#define TEMP_MIN 16
#define TEMP_MAX 25
#define PH_MIN 6
#define PH_MAX 8
#define TURB_MIN 12
#define TURB_MAX 24

// Definición de pines y dimensiones para el OLED
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Definición de pines utilizados por el módulo transceptor LoRa
#define MISO 19 
#define MOSI 27 
#define DIO0 26 
#define RST 14  
#define SCK 5   
#define SS 18

// Configuración para Europa
#define BAND 866E6

void processPackets();
void loraBegin();
void loraOled();
void ledPanelBegin();
void ledCarousel();
String handleWaterRequest();


#endif