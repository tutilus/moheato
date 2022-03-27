# Configuration library

Objectif est de définir les informations de configuration et de les stocker dans la mémoire non volatile de l'ESP8266.

## A FAIRE

- Créer un type MQTT
- Créer un type NETWORK
- (autre type à venir)

Récupérer les infos dans le fichier JSON à l'instanciation
(LittleFS)
Persistance les infos dans le fichier JSON avec une fonction save()
Utiliser la lib ArduinoJson [JsonConfigFile.ino](https://arduinojson.org/v6/example/config/)
Fonction de mise à jour et getter des données (par type)
