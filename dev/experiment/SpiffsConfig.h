// This is a sample file from the book "Mastering ArduinoJson"
// https://arduinojson.org/book/
// Copyright 2017-2019 Benoit Blanchon

#pragma once

#include <ArduinoJson.h>

// File format:
//
// {
//   "access_points": [
//     {
//       "ssid": "SSID1",
//       "passphrase": "PASSPHRASE1"
//     },
//     {
//       "ssid": "SSID2",
//       "passphrase": "PASSPHRASE2"
//     },
//     {
//       "ssid": "SSID3",
//       "passphrase": "PASSPHRASE3"
//     },
//     {
//       "ssid": "SSID4",
//       "passphrase": "PASSPHRASE4"
//     }
//   ],
//   "server": {
//     "host": "www.example.com",
//     "path": "/upload",
//     "username": "admin",
//     "password": "secret"
//   }
// }

// Stores the configuration of a single AP
struct ApConfig {
  char ssid[32];
  char passphrase[64];

  void load(JsonObjectConst);
  void save(JsonObject) const;
};

// Stores the server configuration
struct ServerConfig {
  char host[32];
  char path[32];
  char username[32];
  char password[32];

  void load(JsonObjectConst);
  void save(JsonObject) const;
};

//Test case
struct WaterTank {
  char volume_level[32]; //"Volume"

  void load(JsonObjectConst);
  void save(JsonObject) const;
}

// Stores the complete configuration
struct Config {
  static const int maxAccessPoints = 4;
  ApConfig accessPoint[maxAccessPoints];
  int accessPoints = 0;
  ServerConfig server;

  void load(JsonObjectConst);
  void save(JsonObject) const;
};

bool serializeConfig(const Config &config, Print &dst);
bool deserializeConfig(Stream &src, Config &config);
