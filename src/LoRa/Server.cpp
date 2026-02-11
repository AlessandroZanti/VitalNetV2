#include "LoRa/Server.h"

// Inicialización del arreglo de rutas
Route routes[] = {
  // Rutas HTML...
  {"/inicio", "/pages/inicio.html", "text/html"},
  {"/info-boya", "/pages/info-boya.html", "text/html"},
  {"/lista-boyas", "/pages/lista-boyas.html", "text/html"},
  {"/info-torreta", "/pages/info-torreta.html", "text/html"},
  {"/lista-torretas", "/pages/lista-torretas.html", "text/html"},
  {"/configuracion", "/pages/configuracion.html", "text/html"},
  // Rutas JS...
  {"/js/script.js", "/js/script.js", "application/javascript"},
  {"/js/highcharts.min.js", "/js/highcharts.min.js", "application/javascript"},
  // Rutas CSS...
  {"/css/index.min.css", "/css/index.min.css", "text/css"},
  // imágenes SVG...
  {"/images/arrow-right.svg", "/images/arrow-right.svg", "image/svg+xml"},
  {"/images/vitalnet-logo.svg", "/images/vitalnet-logo.svg", "image/svg+xml"},
  {"/images/luis-amigo.png", "/images/luis-amigo.png", "image/png"},
  {"/images/teleco-games.webp", "/images/teleco-games.webp", "image/webp"},
  {"/images/vitalnet-logo-color.svg", "/images/vitalnet-logo-color.svg", "image/svg+xml"},
};

// Obtener la dirección MAC del ESP32 y crea un nombre único para el receptor basado en parte de la MAC
String mac_address = WiFi.macAddress();
String microcontroller = "VitalNet-Reciever-" + mac_address.substring(6, 8) + mac_address.substring(9, 11);

AsyncWebServer server(80);

// Función para generar el JSON con la información del sistema
String IRAM_ATTR generateSystemInfoJson() {
  // Crear un objeto JSON
  DynamicJsonDocument doc(1024);

  // Información del sistema
  JsonObject systemInfo = doc.createNestedObject("system_info");
  systemInfo["chip_id"] = ESP.getEfuseMac();
  systemInfo["free_heap"] = ESP.getFreeHeap();
  systemInfo["chip_revision"] = ESP.getChipRevision();
  systemInfo["sdk_version"] = ESP.getSdkVersion();

  // Información de la red WiFi
  JsonObject wifiInfo = doc.createNestedObject("wifi_info");
  wifiInfo["mac_address"] = WiFi.macAddress();
  wifiInfo["ssid"] = WiFi.SSID();
  wifiInfo["rssi"] = WiFi.RSSI();

  // Información de uso de CPU
  JsonObject cpuInfo = doc.createNestedObject("cpu_info");
  cpuInfo["free_stack_space"] = uxTaskGetStackHighWaterMark(NULL);

  // Crear una cadena JSON
  String json;
  serializeJson(doc, json);
  yield();
  return json;
}

String IRAM_ATTR getWifiScanJson() {
  if(WiFi.status()==1||WiFi.status()==6||WiFi.status()==5){
    WiFi.disconnect();
  }    
  int contador=0;
  int n;
  String json;
  do {
      n = WiFi.scanComplete();
      json = "{\"scan_result\":[";
      if (n == -2) {
        WiFi.scanNetworks(true,false);
      } else if (n) {
        for (int i = 0; i < n; ++i) {
          if (i) json += ",";
          json += "{";
          json += "\"RSSI\":" + String(WiFi.RSSI(i));
          json += ",\"SSID\":\"" + WiFi.SSID(i) + "\"";
          json += "}";
        } //{ RSSI: -65, SSID: "Lowi46CC" },
        WiFi.scanDelete();
        if (WiFi.scanComplete() ==0) {
          WiFi.scanNetworks(true);
        }
      }
      json += "]}";
  }while(n==-2);//||n==0);
  yield();
  return json;
}

void createAccessPoint() {
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  // Configura y activa el punto de acceso WiFi con los datos especificados
  WiFi.softAP(microcontroller, "Adminxxx1");
  // Informa que el punto de acceso ha sido configurado y muestra su dirección IP
  Serial.print("IP del punto de acceso: ");
  Serial.println(WiFi.softAPIP());
  // TODO: Debería mostrar en la pantalla el IP creado como punto de acceso
}

void connectToWifi() {
  Preferences preferences;
  preferences.begin("wifi_settings", false);

  // Leer SSID y contraseña de las preferencias
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  
  // Si las credenciales no están vacías, intentar conectarse
  if (ssid.length() > 0 && password.length() > 0) {
    Serial.println("Conectando a WiFi con las credenciales guardadas...");
    WiFi.begin(ssid.c_str(), password.c_str());
    int connectionAttempt = 0;
    while (WiFi.status() != WL_CONNECTED && connectionAttempt < 10) { // Intentar hasta 10 veces
      delay(500);
      Serial.println("Conectando...");
      connectionAttempt++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      // IP asignada dentro de la red WiFi
      Serial.print("IP dentro de la red wifi: ");
      Serial.println(WiFi.localIP());
      // Confirma la conexión exitosa y muestra la 
      Serial.print("Conectado a la red wifi: ");
      Serial.println(ssid);
    } else {
      Serial.println("Error al conectar a la red WiFi. Verifica las credenciales o la disponibilidad de la red.");
    }
  } else {
    Serial.println("No se encontraron credenciales guardadas.");
  }

  // Cerrar el espacio de preferencias
  preferences.end();
}

void serverRoutes() {
  // verifica que SPIFFS este funcionando correctamente.
  if (!SPIFFS.begin(true)) {
    Serial.println("Ocurrió un error al montar SPIFFS");
    return;
  }

  // informacion obtenida por una torreta.
  server.on("/air-info", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("air-info route fetched");
    String response = handleAirRequest();
    request->send(200, "application/json", response);
  });

  // informacion obtenida por una boya.
  server.on("/water-info", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("water-info route fetched");
    String response = handleWaterRequest();
    request->send(200, "application/json", response);
  });

  // informacion del sistema.
  server.on("/system-info", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = generateSystemInfoJson();
    request->send(200, "application/json", json);
  });

  // informacion del sistema.
  server.on("/connections-info", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Escaneando conecciones wifi");
    String json = getWifiScanJson(); // Llama a la función para obtener el JSON
    request->send(200, "application/json", json); // Envía el JSON como respuesta
  });

  // informacion del sistema.
  server.on("/connections", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("ssid") && request->hasParam("password")) {
      // Obtener SSID y contraseña de los parámetros
      String ssid = request->getParam("ssid")->value();
      String password = request->getParam("password")->value();
      
      // Guardar SSID y contraseña en las preferencias
      Preferences preferences;
      
      preferences.begin("wifi_settings", false);
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
      preferences.end();
      
      // Conectar a la red WiFi con las nuevas credenciales
      connectToWifi();
      
      // Enviar respuesta de éxito
      request->send(200, "text/plain", "Se configuró la conexión correctamente.");              
    } else {
      // Enviar respuesta de error si faltan parámetros
      request->send(400, "text/plain", "Parámetros faltantes");
    } 
  });

  // Registra las rutas en el servidor web
  for (auto& route : routes) {
    server.on(route.uri, HTTP_GET, [&route](AsyncWebServerRequest *request) {
      request->send(SPIFFS, route.path, route.contentType); // Envía el archivo solicitado desde SPIFFS
    });
  }
}

void defaultServices() {

  createAccessPoint();
  
  if (!SPIFFS.begin(true)) {
    Serial.println("Ocurrió un error al montar SPIFFS");
    return;
  }

  // Ruta de registro.
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/pages/login.html", "text/html");
  });

  // Manejo del registro.
  server.on("/login", HTTP_GET, [](AsyncWebServerRequest *request) {

    // Obtener los parámetros del formulario
    if (request->hasParam("username") && request->hasParam("password")) {
      
      String username = request->getParam("username")->value();
      String password = request->getParam("password")->value();

      // Verificar credenciales (aquí deberías comparar con tus credenciales almacenadas)
      if (username == "usuario" && password == "contraseña") {
        serverRoutes();
        request->send(200, "text/plain", "Inicio de sesión exitoso");              
      } else {
        request->send(401, "text/plain", "Credenciales incorrectas");
      }
    } else {
      request->send(400, "text/plain", "Parámetros faltantes");
    }
  });

  // Iniciar el servidor
  server.begin();
}