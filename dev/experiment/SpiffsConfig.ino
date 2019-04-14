// This is a sample file from the book "Mastering ArduinoJson"
// https://arduinojson.org/book/
// Copyright 2017-2019 Benoit Blanchon
//
// This example shows how to store your project configuration in a JSON file.
// The file is stored in SPIFFS, so you need something compatible (like an
// ESP8266) to run this code.

#include <FS.h>

#include "Config.h"

const char *filename = "/config.json";
Config config;

// Loads the configuration from a file on SPIFFS
bool loadFile(const char *filename, Config &config) {
  // Open file for reading
  File file = SPIFFS.open(filename, "r");

  // This may fail if the file is missing
  if (!file) {
    Serial.println(F("Failed to open config file"));
    return false;
  }

  // Parse the JSON object in the file
  bool success = deserializeConfig(file, config);

  // This may fail if the JSON is invalid
  if (!success) {
    Serial.println(F("Failed to deserialize configuration"));
    return false;
  }

  return true;
}

// Saves the configuration to a file on SPIFFS
void saveFile(const char *filename, const Config &config) {
  // Open file for writing
  File file = SPIFFS.open(filename, "w");
  if (!file) {
    Serial.println(F("Failed to create config file"));
    return;
  }

  // Serialize JSON to file
  bool success = serializeConfig(config, file);
  if (!success) {
    Serial.println(F("Failed to serialize configuration"));
  }
}

// Prints the content of a file to the Serial
void printFile(const char *filename) {
  // Open file for reading
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    Serial.println(F("Failed to open config file"));
    return;
  }

  // Extract each by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();
}

void setup() {
  // Initialize serial port
  Serial.begin(9600);
  while (!Serial)
    continue;

  // Mount SPIFFS
  delay(1000);
  if (!SPIFFS.begin()) {
    Serial.println(F("Failed to mount SPIFFS"));
    return;
  }

  // Load configuration
  bool loaded = loadFile(filename, config);

  if (!loaded) {
    Serial.println(F("Using default config"));
    strcpy(config.server.host, "example.com");
    strcpy(config.server.path, "/resource");
    strcpy(config.server.username, "admin");
    strcpy(config.server.password, "s3cr3t");
    strcpy(config.accessPoint[0].ssid, "SSID 1");
    strcpy(config.accessPoint[0].passphrase, "Passphrase 1");
    strcpy(config.accessPoint[1].ssid, "SSID 2");
    strcpy(config.accessPoint[1].passphrase, "Passphrase 2");
    config.accessPoints = 2;
  }

  // Save configuration
  saveFile(filename, config);

  // Dump config file
  printFile(filename);

  if (!loaded)
    Serial.println(F("Restart to load the config"));
  else
    Serial.println(F("Done!"));
}

void loop() {
}
