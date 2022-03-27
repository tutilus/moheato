# Mo-Heat-O

ESP8266 board heating system controller. It contains 2 libraries (1) to configure the Web Server access and persist data (2) to manage the heaters (3 different rooms).

## Default

By default, server is up in Access Point on `192.168.4.22` IP address and SSID `moheato-network` (everything could be change into [config.h](lib/config/src/config.h)); but it can be configure in STA using config API.

## Description

API available are:

- Config API to configure ESP8266
- Heaters API to manage heaters
- General API as healthcheck

Everything is described into [swagger.yaml](api/swagger/swagger.yaml)