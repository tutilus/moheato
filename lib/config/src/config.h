/**
 * Configuration library to manage a config file into
 * the LittleFS memory. 
 * Copyright (c) 2006 Tutilus.  All right reserved.
 * @created_at: 23-03-2022
 * 
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <string.h>

#define DEFAULT_MQTT_CHANNEL "/moheato/logs"
#define DEFAULT_MQTT_NETWORK "0.0.0.0"
#define DEFAULT_MQTT_PORT 1183
#define DEFAULT_NETWORK_SSID "moheato-network"
#define DEFAULT_NETWORK_PWD  "RSbbmZe4yeBe4w"
#define DEFAULT_NETWORK_CHANNEL 1
#define DEFAULT_NETWORK_AP_MODE 1 // AP activated mode
#define DEFAULT_NETWORK_HIDDEN 0

typedef struct {
    String network;
    String channel;
    int port;
} t_mqtt_config;

typedef struct {
    String ssid;
    String password;
    int channel;
    bool ap_mode;
    bool hidden;
} t_network_config;

class Config
{
private:
    char path[64];
    t_mqtt_config mqtt_cfg;
    t_network_config network_cfg;

public:
    Config(const char* filename);
    ~Config();

    // Get parameters for MQTT
    t_mqtt_config getMqttParams(void);

    // Get parameters for Network
    t_network_config* getNetworkParams(void);

    void setNetworkSsid(const String ssid);
    void setNetworkPassword(const String password);
    void setNetworkChannel(const int channel);
    void setNetworkModeAp(const bool active);
    void setNetworkHidden(const bool active);

    void setMqttNetwork(const String network);
    void setMqttChannel(const String channel);
    void setMqttPort(const int port);

    // Set all value to default
    void setDefault(void);
    
    // Save current data to file
    void save(void);
};

#endif // CONFIG_H