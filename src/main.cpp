// Importaciones predeterminadas de Arduino
#include <arduino.h>

#include "LoRa/LoRa.h"
#include "LoRa/Server.h"


TaskHandle_t sensorTask;
void updateTemp(void *pvParameters){
  Serial.println(xPortGetCoreID());

  for(;;) {
    getTemp();
  }
  unsigned long lastTempUpdate = 0;   // Última vez que se ejecutó getTemp()
  unsigned long lastShowUpdate = 0;   // Última vez que se ejecutó showTemp()
  const unsigned long tempInterval = 500;    // Intervalo de 500 ms para getTemp()
  const unsigned long showInterval = 10000;  // Intervalo de 10,000 ms para showTemp()
  for(;;) {
    unsigned long currentMillis = millis();
    // Ejecutar getTemp() cada 500 ms
    if (currentMillis - lastTempUpdate >= tempInterval) {
      lastTempUpdate = currentMillis;
      getTemp();
      Serial.println("Get Temp"); //? Quitar en un futuro, es para verificar que esta funcion se ejacuta cada 500 ms y get temp 10000ms
    }
    // Ejecutar showTemp() cada 10,000 ms
    if (currentMillis - lastShowUpdate >= showInterval) {
      lastShowUpdate = currentMillis;
      showTemp();
    }
  }
    vTaskDelay(100 / portTICK_PERIOD_MS); // Espera 500 ms entre cada iteración.
}

// TaskHandle_t LedPanelTask;
// void updateLedPanel(void *pvParameters){
//   Serial.println(xPortGetCoreID());
//   for(;;) {
//     ledCarousel();
//     // todo: crear una function para manejar la actualización de la pantalla.
//   }
// }

TaskHandle_t MessagesTask;
void getLoraMessages(void *pvParameters){
  Serial.println(xPortGetCoreID());
  for(;;) {
    int packetSize = LoRa.parsePacket();
      // Verifica si hay paquetes LoRa disponibles para leer
    if (packetSize) {
      processPackets();
    }
  }
}

void setup() { 
  // Inicializa el Monitor Serial
  Serial.begin(115200); // Configura la velocidad de datos en bits por segundo para la transmisión serial
  while (!Serial) continue;

  WiFi.softAPdisconnect(true); // Desahabilitar el punto de acceso.
  
  connectToWifi();

  loraBegin(); // Inicia la configuración del módulo LoRa.
  ledPanelBegin(); // inicia el panel led.
  airSensorBegin(); // inicia el sensor de aire.
  
  defaultServices(); // servicios necesarios.

  xTaskCreatePinnedToCore(
    updateTemp,
    "Actualizar los valores obtenidos por el sensor de aire", 
    2000,
    NULL,
    3,
    &sensorTask,
    0
  );

  // xTaskCreatePinnedToCore(
  //   updateLedPanel,
  //   "Actualizar y mostrar el contenido de la pantalla", 
  //   8500,
  //   NULL,
  //   2,
  //   &LedPanelTask,
  //   1
  // );

  xTaskCreatePinnedToCore(
    getLoraMessages,
    "Comprobar si llegó información a traves de LoRa y manejar el contenido", 
    3000,
    NULL,
    1,
    &MessagesTask,
    1
  );
}

void loop() {} // silence.