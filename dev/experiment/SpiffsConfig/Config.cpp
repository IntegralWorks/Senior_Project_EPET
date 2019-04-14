// This is a sample file from the book "Mastering ArduinoJson"
// https://arduinojson.org/book/
// Copyright 2017-2019 Benoit Blanchon

#include "Config.h"

void ApConfig::save(JsonObject obj) const {
  obj["ssid"] = ssid;
  obj["passphrase"] = passphrase;
}

void ApConfig::load(JsonObjectConst obj) {
  strlcpy(ssid, obj["ssid"] | "", sizeof(ssid));
  strlcpy(passphrase, obj["passphrase"] | "", sizeof(passphrase));
}

void ServerConfig::save(JsonObject obj) const {
  obj["username"] = username;
  obj["password"] = password;
  obj["host"] = host;
  obj["path"] = path;
}

void ServerConfig::load(JsonObjectConst obj) {
  strlcpy(username, obj["username"] | "", sizeof(username));
  strlcpy(password, obj["password"] | "", sizeof(password));
  strlcpy(host, obj["host"] | "", sizeof(host));
  strlcpy(path, obj["path"] | "", sizeof(path));
}

void Config::load(JsonObjectConst obj) {
  // Read "server" object
  server.load(obj["server"]);

  // Get a reference to the access_points array
  JsonArrayConst aps = obj["access_points"];

  // Extract each access points
  accessPoints = 0;
  for (JsonObjectConst ap : aps) {
    // Load the AP
    accessPoint[accessPoints].load(ap);

    // Increment AP count
    accessPoints++;

    // Max reach?
    if (accessPoints >= maxAccessPoints)
      break;
  }
}

void Config::save(JsonObject obj) const {
  // Add "server" object
  server.save(obj.createNestedObject("server"));

  // Add "acces_points" array
  JsonArray aps = obj.createNestedArray("access_points");

  // Add each acces point in the array
  for (int i = 0; i < accessPoints; i++)
    accessPoint[i].save(aps.createNestedObject());
}

bool serializeConfig(const Config &config, Print &dst) {
  DynamicJsonDocument doc(512);

  // Create an object at the root
  JsonObject root = doc.to<JsonObject>();

  // Fill the object
  config.save(root);

  // Serialize JSON to file
  return serializeJsonPretty(doc, dst) > 0;
}

bool deserializeConfig(Stream &src, Config &config) {
  DynamicJsonDocument doc(1024);

  // Parse the JSON object in the file
  DeserializationError err = deserializeJson(doc, src);
  if (err)
    return false;

  config.load(doc.as<JsonObject>());
  return true;
}
