/**
 * config.cpp - Configuration librairy for ESP8266
 * Copyright (c) 2006 Tutilus.  All right reserved.
 * 
 * @created_at: 24-03-2022
 */

#include "config.h"

Config::Config(const char* config_path)
{
  LittleFS.begin();
  sprintf(this->path, "%s", config_path);
  if (LittleFS.exists(config_path)) {
    File cfgfile = LittleFS.open(config_path, "r");
    if (! cfgfile) {
      Serial.println(F("Failed to open config file"));
      return;
    }
    // Get all data from the file
    StaticJsonDocument<512> cfg_json;
    DeserializationError err = deserializeJson(cfg_json, cfgfile);
    if (err) {
      Serial.println(F("Failed to JSON config file"));
      return;
    }
    // Set MQTT data from JSON file
    this->mqtt_cfg.channel = cfg_json["mqtt-channel"] | DEFAULT_MQTT_CHANNEL;
    this->mqtt_cfg.network = cfg_json["mqtt-network"] | DEFAULT_MQTT_NETWORK;
    this->mqtt_cfg.port = cfg_json["mqtt-port"] | DEFAULT_MQTT_PORT;

    // Set Network data from JSON file
    this->network_cfg.ssid = cfg_json["network-ssid"] | DEFAULT_NETWORK_SSID;
    this->network_cfg.password = cfg_json["network-pwd"] | DEFAULT_NETWORK_PWD;
    this->network_cfg.channel = cfg_json["network-channel"] | DEFAULT_NETWORK_CHANNEL;
    this->network_cfg.ap_mode = cfg_json.containsKey("network-ap-mode") ? cfg_json["network-ap-mode"] : DEFAULT_NETWORK_AP_MODE;
    this->network_cfg.hidden = cfg_json.containsKey("network-hidden") ? cfg_json["network-hidden"] : DEFAULT_NETWORK_HIDDEN;
  } else {
    // Initialize at default value
    setDefault();
  }
}

Config::~Config() {
  LittleFS.end();
}

t_mqtt_config Config::getMqttParams(void) {
  return this->mqtt_cfg;
}

t_network_config* Config::getNetworkParams(void) {
  return &this->network_cfg;
}

// Setter for Network config
void Config::setNetworkSsid(const String ssid) {
  this->network_cfg.ssid = ssid;
}
void Config::setNetworkPassword(const String password) {
  this->network_cfg.password = password;
}
void Config::setNetworkChannel(const int channel) {
  this->network_cfg.channel = channel;
}
void Config::setNetworkModeAp(const bool active) {
  this->network_cfg.ap_mode = active;
}
void Config::setNetworkHidden(const bool active) {
  this->network_cfg.hidden = active;
}

void Config::setMqttNetwork(const String network) {
  this->mqtt_cfg.network = network;
}
void Config::setMqttChannel(const String channel) {
  this->mqtt_cfg.channel = channel;
}
void Config::setMqttPort(const int port) {
  this->mqtt_cfg.port = port;
}

void Config::setDefault(void) {
    // Set MQTT data from JSON file
    this->mqtt_cfg.channel = DEFAULT_MQTT_CHANNEL;
    this->mqtt_cfg.network = DEFAULT_MQTT_NETWORK;
    this->mqtt_cfg.port =  DEFAULT_MQTT_PORT;

    // Set Network data from JSON file
    this->network_cfg.ssid = DEFAULT_NETWORK_SSID;
    this->network_cfg.password = DEFAULT_NETWORK_PWD;
    this->network_cfg.channel = DEFAULT_NETWORK_CHANNEL;
    this->network_cfg.ap_mode = DEFAULT_NETWORK_AP_MODE;
    this->network_cfg.hidden = DEFAULT_NETWORK_HIDDEN;
}

void Config::save(void) {
    // Delete the current file
    LittleFS.remove(this->path);

    File file = LittleFS.open(this->path, "w+");
    if (!file) {
        Serial.println(F("Failed to create file"));
        return;
    }

    StaticJsonDocument<512> cfg_json;

    cfg_json["mqtt-channel"] = this->mqtt_cfg.channel;
    cfg_json["mqtt-network"] = this->mqtt_cfg.network;
    cfg_json["mqtt-port"] = this->mqtt_cfg.port;

    cfg_json["network-ssid"] = this->network_cfg.ssid;
    cfg_json["network-pwd"] = this->network_cfg.password;
    cfg_json["network-channel"] = this->network_cfg.channel;
    cfg_json["network-ap-mode"] = this->network_cfg.ap_mode;
    cfg_json["network-hidden"] = this->network_cfg.hidden;

    if (serializeJson(cfg_json, file) == 0) {
        Serial.println(F("Failed ti write config to file"));
        return;
    }
    file.close();
}