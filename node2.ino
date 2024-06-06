#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

#define ss 16
#define rst 14
#define dio0 26

void setup() {
  dht.begin();
  Serial.begin(115200);

  LoRa.setPins(ss, rst, dio0);
  while (!LoRa.begin(866E6)) {
    Serial.print(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  Serial.print("Sending packet: ");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C, ");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  LoRa.beginPacket();
  LoRa.print(temperature);
  LoRa.print(" ");
  LoRa.print(humidity);
  LoRa.endPacket();

  delay(2500);  // Send every 10 seconds
}