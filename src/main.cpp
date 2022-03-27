/**
 * Electronic Heating Controller API
 * 
 *  Created on: 22/11/2018
 *  
 *  - 3 zones to control through (D1,D2) (D5,D6) (D7,D8)
 *  - log on MQTT 
 *  - control through REST API
 *
 *  TODO:
 *   - Add POST on server /api to config NTP, MQTT or WiFi
 *   - Utiliser SPIFFS pour stocker les infos 
 */
 
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include "config.h"
#include "heatrol.h"

#define ZONE_NR 3
#define CONNECT_TIME_NR 15

Config* config;
AsyncWebServer server(80);
Heatrol heaters[ZONE_NR];

// =========== Utilities functions =======
void setUpHeaters() {
  // TODO: Persiste into config file. 
  // TODO: Add a function into Heater to getData() setFromData()
  heaters[0].begin(D1, D2, "room #1");
  heaters[1].begin(D5, D6, "room #2");
  heaters[2].begin(D7, D8, "room #3");
}


void setUpNetworkAP(t_network_config* net_cfg) {
  // IP Adresses for Acccess Point
  IPAddress local_IP(192,168,4,22);
  IPAddress gateway(192,168,4,9);
  IPAddress subnet(255,255,255,0);
  Serial.print("Setting soft-AP configuration ...");

  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ...");
  Serial.println(WiFi.softAP(
    net_cfg->ssid,
    net_cfg->password,
    net_cfg->hidden) ? "Ready" : "Failed!");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
}

void setUpNetworkSTA(t_network_config* net_cfg) {
  Serial.println("Setting STA configuration ...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(
    net_cfg->ssid,
    net_cfg->password);
  // Wait connection is established
  Serial.print("Connecting to SSID ...");
  for (uint8_t i = 0; i < CONNECT_TIME_NR; i++) {
    if (WiFi.status() == WL_CONNECTED) break;
    delay(1000); 
    Serial.print(".");
  }
  // Connection establish correctly
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connection established!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    // Failed to connect ! 
    Serial.println("Failed to connect ! Err=" + String(WiFi.status()));
    // Set back default value (without saving to file) and setup AP
    config->setDefault();
    setUpNetworkAP(config->getNetworkParams());
    Serial.println("Set back to default AP for this session.");
  }
}

void setUpNetwork() {
  t_network_config* net_cfg = config->getNetworkParams();
  if (net_cfg->ap_mode) { 
    setUpNetworkAP(net_cfg);
  } else {
    setUpNetworkSTA(net_cfg);
  }
}

// ============= API =====================
void handleNotFound(AsyncWebServerRequest* request) {
  String message = "Resquest Not Found";
  request->send(404, "text/plain", message);  
}

// GET /api/config
void handleConfigShow(AsyncWebServerRequest* request) {
  AsyncResponseStream* response = request->beginResponseStream("application/json");
  StaticJsonDocument<256> json;

  JsonObject network_obj = json.createNestedObject("network");
  t_network_config* net_cfg = config->getNetworkParams();
  network_obj["ssid"] = net_cfg->ssid;
  network_obj["mode_app"] = net_cfg->ap_mode;
  network_obj["channel"] = net_cfg->channel;
  serializeJson(json, *response);
  response->setCode(200);
  request->send(response);
}

// PATCH /api/config/network
void handleConfigUpdateNetwork(AsyncWebServerRequest* request) {
  bool changed = false;

  // Check arguments
  if (request->hasArg("ssid")) {
    config->setNetworkSsid(request->arg("ssid"));
    changed = true;
  }
  if (request->hasArg("password")) {
    config->setNetworkPassword(request->arg("password"));
    changed = true;
  }
  if (request->hasArg("mode_ap")) {
    config->setNetworkModeAp(request->arg("mode_ap").toInt());
    changed = true;
  }
  if (request->hasArg("hidden")) {
    config->setNetworkModeAp(request->arg("hidden").toInt());
    changed = true;
  }
  if (changed) {
    Serial.println("Save modification in file.");
    // Save information into file.
    config->save();
    request->send(201);
  } else {
    Serial.println("Nothing to do");
    request->send(200);
  }
}

void handleConfigReload(AsyncWebServerRequest* request) {
    // Update le network with the new information.
    setUpNetwork();
    request->send(200);
}

void handleHeatersData(AsyncWebServerRequest* request) {
  AsyncResponseStream* response = request->beginResponseStream("application/json");
  StaticJsonDocument<512> json;

  JsonArray heatersJson = json.createNestedArray("heaters");
  
  for (uint8 z = 0; z < ZONE_NR; z++) {
    StaticJsonDocument<256> json;
    Heatrol* heater = &heaters[z];
    json["name"] = heater->getName();
    json["mode-id"] = heater->getOrder();
    json["mode"] = heater->getOrderLabel();
    json["default-mode-id"] = heater->getDefaultOrder();
    json["default-mode"] = heater->getDefaultOrderLabel();
    heatersJson.add(json);
  }

  serializeJson(heatersJson, *response);
  Serial.println("GET /heaters");
  response->setCode(200);
  request->send(response);
}

void handleHeatersOptions(AsyncWebServerRequest* request) {
  AsyncResponseStream* response = request->beginResponseStream("text/plain");

  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Access-Control-Max-Age", "10000");
  response->addHeader("Access-Control-Allow-Methods", "POST,PATCH,GET,OPTIONS");
  response->addHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");

  request->send(response);
}

/**
 * Show data for the heater in provided zone.
 */
void handleHeaterData(AsyncWebServerRequest* request, const int zone) {
  AsyncResponseStream* response = request->beginResponseStream("application/json");

  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Access-Control-Allow-Methods", "POST,PATCH,GET");
  response->addHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");

  if (zone > 0 && zone < ZONE_NR) {
    StaticJsonDocument<256> json;
    Heatrol* heater = &heaters[zone];
    json["name"] = heater->getName();
    json["mode-id"] = heater->getOrder();
    json["mode"] = heater->getOrderLabel();
    json["default-mode-id"] = heater->getDefaultOrder();
    json["default-mode"] = heater->getDefaultOrderLabel();
    serializeJson(json, *response);
    response->setCode(200);
  } else {
    response->setCode(404);
  }
  request->send(response);
}

void handleHeaterDefault(AsyncWebServerRequest* request, const int zone) {
  AsyncResponseStream* response = request->beginResponseStream("application/json");

  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Access-Control-Allow-Methods", "POST,PATCH,GET");
  response->addHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");

  if (zone > 0 && zone < ZONE_NR) {
    Heatrol* heater = &heaters[zone];
    heater->toDefault();
    response->setCode(200);
  } else {
    response->setCode(404);
  }
  request->send(response);
}

void handleHeaterUpdate(AsyncWebServerRequest* request, const int zone) {
  AsyncResponseStream* response = request->beginResponseStream("application/json");

  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Access-Control-Allow-Methods", "POST,PATCH,GET");
  response->addHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");

  if (zone >= 0 && zone < ZONE_NR) {
    Heatrol* heater = &heaters[zone];
    // Get the mode parameter
    // Check arguments
    if (request->hasArg("mode")) {
      const hzOrder_t mode = (hzOrder_t)request->arg("mode").toInt();
      // Check if mode exists
      if (mode < 0 || mode >= 4) {
        request->send(400, "text/plain", "Mode is out of range.");
        return;
      }
      heater->setOrder(mode);
      Serial.println("Heater #" + String(zone+1) + ": Change to " + heater->getOrderLabel());
    }   
    response->setCode(200);
  } else {
    response->setCode(404);
  }
  request->send(response);
}

// ========= SETUP ===============

void setup() {
  Serial.begin(57600);
  
  //------------ Config file ----------------
  Serial.println("Testing ...");
  config = new Config("/config2.json");
  Serial.println("Configuration créé.");

  // ------- Set Network WiFi ----------------
  setUpNetwork();

  // -------- Heaters -----------------
  // Initialize heaters for each zones.
  setUpHeaters();

  //---------- Web Server -------------
  // General API
  server.on("/api/heathcheck", HTTP_GET, [](AsyncWebServerRequest* request) { request->send(200, "text/plain", "true"); });
  // Config API
  server.on("/api/config/reload", HTTP_POST, handleConfigReload);
  server.on("/api/config/network", HTTP_PATCH, handleConfigUpdateNetwork);
  server.on("/api/config", HTTP_GET, handleConfigShow);
  // Heaters API
  for (uint8_t i = 0; i < ZONE_NR; i++) {
    server.on(("/api/heaters/" + String(i + 1) + "/default").c_str(), HTTP_POST, [z=i](AsyncWebServerRequest* request) { handleHeaterDefault(request, z); });
    server.on(("/api/heaters/" + String(i + 1)).c_str(), HTTP_OPTIONS, handleHeatersOptions);
    server.on(("/api/heaters/" + String(i + 1)).c_str(), HTTP_GET, [z=i](AsyncWebServerRequest* request) { handleHeaterData(request, z); });
    server.on(("/api/heaters/" + String(i + 1)).c_str(), HTTP_POST, [z=i](AsyncWebServerRequest* request) { handleHeaterUpdate(request, z); });
  }
  server.on("/api/heaters", HTTP_OPTIONS, handleHeatersOptions);
  server.on("/api/heaters", HTTP_GET, handleHeatersData);

  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() { }