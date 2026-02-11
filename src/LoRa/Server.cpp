#include "LoRa/Server.h"
#include <mbedtls/sha256.h>

// Helper to hash password
String hashPassword(String password) {
  byte shaResult[32];
  mbedtls_sha256_context ctx;
  mbedtls_sha256_init(&ctx);
  mbedtls_sha256_starts(&ctx, 0);
  mbedtls_sha256_update(&ctx, (const unsigned char*)password.c_str(), password.length());
  mbedtls_sha256_finish(&ctx, shaResult);
  mbedtls_sha256_free(&ctx);

  String hash = "";
  for (int i = 0; i < 32; i++) {
    char str[3];
    sprintf(str, "%02x", (int)shaResult[i]);
    hash += str;
  }
  return hash;
}

// Helper to check if a user exists and verify password
bool verifyUser(String username, String password) {
  if (!SPIFFS.exists("/users.json")) return false;
  File file = SPIFFS.open("/users.json", "r");
  if (!file) return false;

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();
  if (error) return false;

  JsonArray users = doc.as<JsonArray>();
  String hashedPassword = hashPassword(password);

  for (JsonObject user : users) {
    if (user["username"] == username && user["password"] == hashedPassword) {
      return true;
    }
  }
  return false;
}

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

// ... (keep generateSystemInfoJson, getWifiScanJson, createAccessPoint, connectToWifi as they are)
String IRAM_ATTR generateSystemInfoJson() {
  DynamicJsonDocument doc(1024);
  JsonObject systemInfo = doc.createNestedObject("system_info");
  systemInfo["chip_id"] = ESP.getEfuseMac();
  systemInfo["free_heap"] = ESP.getFreeHeap();
  systemInfo["chip_revision"] = ESP.getChipRevision();
  systemInfo["sdk_version"] = ESP.getSdkVersion();
  JsonObject wifiInfo = doc.createNestedObject("wifi_info");
  wifiInfo["mac_address"] = WiFi.macAddress();
  wifiInfo["ssid"] = WiFi.SSID();
  wifiInfo["rssi"] = WiFi.RSSI();
  JsonObject cpuInfo = doc.createNestedObject("cpu_info");
  cpuInfo["free_stack_space"] = uxTaskGetStackHighWaterMark(NULL);
  String json;
  serializeJson(doc, json);
  yield();
  return json;
}

String IRAM_ATTR getWifiScanJson() {
  if(WiFi.status()==1||WiFi.status()==6||WiFi.status()==5){
    WiFi.disconnect();
  }    
  int n;
  String json;
  do {
      n = WiFi.scanComplete();
      json = "{\"scan_result\":[";
      if (n == -2) {
        WiFi.scanNetworks(true,false);
      } else if (n > 0) {
        for (int i = 0; i < n; ++i) {
          if (i) json += ",";
          json += "{";
          json += "\"RSSI\":" + String(WiFi.RSSI(i));
          json += ",\"SSID\":\"" + WiFi.SSID(i) + "\"";
          json += "}";
        }
        WiFi.scanDelete();
      }
      json += "]}";
  }while(n==-2);
  yield();
  return json;
}

void createAccessPoint() {
  String mac_address = WiFi.macAddress();
  String microcontroller = "VitalNet-Reciever-" + mac_address.substring(6, 8) + mac_address.substring(9, 11);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(microcontroller, "Adminxxx1");
  Serial.print("IP del punto de acceso: ");
  Serial.println(WiFi.softAPIP());
}

void connectToWifi() {
  Preferences preferences;
  preferences.begin("wifi_settings", false);
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  if (ssid.length() > 0 && password.length() > 0) {
    WiFi.begin(ssid.c_str(), password.c_str());
    int connectionAttempt = 0;
    while (WiFi.status() != WL_CONNECTED && connectionAttempt < 10) {
      delay(500);
      connectionAttempt++;
    }
  }
  preferences.end();
}

void serverRoutes() {
  if (!SPIFFS.begin(true)) return;

  server.on("/air-info", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", handleAirRequest());
  });

  server.on("/water-info", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", handleWaterRequest());
  });

  server.on("/system-info", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", generateSystemInfoJson());
  });

  server.on("/connections-info", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", getWifiScanJson());
  });

  server.on("/connections", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("ssid") && request->hasParam("password")) {
      String ssid = request->getParam("ssid")->value();
      String password = request->getParam("password")->value();
      Preferences preferences;
      preferences.begin("wifi_settings", false);
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
      preferences.end();
      connectToWifi();
      request->send(200, "text/plain", "OK");              
    } else {
      request->send(400, "text/plain", "Missing params");
    } 
  });

  for (auto& route : routes) {
    server.on(route.uri, HTTP_GET, [&route](AsyncWebServerRequest *request) {
      request->send(SPIFFS, route.path, route.contentType);
    });
  }
}

void defaultServices() {
  createAccessPoint();
  if (!SPIFFS.begin(true)) return;

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      request->send(SPIFFS, "/pages/login.html", "text/html");
  });

  server.on("/login", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("username") && request->hasParam("password")) {
      String username = request->getParam("username")->value();
      String password = request->getParam("password")->value();

      if (verifyUser(username, password)) {
        serverRoutes();
        request->send(200, "text/plain", "OK");              
      } else {
        request->send(401, "text/plain", "Invalid credentials");
      }
    } else {
      request->send(400, "text/plain", "Missing params");
    }
  });

  server.on("/register", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
    JsonDocument registerDoc;
    DeserializationError error = deserializeJson(registerDoc, data, len);
    if (error) {
      request->send(400, "text/plain", "Invalid JSON");
      return;
    }

    String username = registerDoc["email"].as<String>(); // Using email as username for this example
    String password = registerDoc["password"].as<String>();
    
    if (username == "" || password == "") {
      request->send(400, "text/plain", "Missing data");
      return;
    }

    JsonDocument dbDoc;
    if (SPIFFS.exists("/users.json")) {
      File file = SPIFFS.open("/users.json", "r");
      deserializeJson(dbDoc, file);
      file.close();
    } else {
      dbDoc.to<JsonArray>();
    }

    JsonArray users = dbDoc.as<JsonArray>();
    
    // Check if user already exists
    for (JsonObject u : users) {
      if (u["username"] == username) {
        request->send(409, "text/plain", "User exists");
        return;
      }
    }

    JsonObject newUser = users.createNestedObject();
    newUser["username"] = username;
    newUser["password"] = hashPassword(password);
    newUser["name"] = registerDoc["name"].as<String>();
    newUser["license"] = registerDoc["license"].as<String>();

    File file = SPIFFS.open("/users.json", "w");
    serializeJson(dbDoc, file);
    file.close();

    request->send(201, "text/plain", "User created");
  });

  server.begin();
}