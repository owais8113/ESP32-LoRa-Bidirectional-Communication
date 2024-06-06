#include <SPI.h>
#include <LoRa.h>
#include "DHT.h"

#define DHTPIN 4      // Selecting the pin at which we have connected DHT11
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);

//define the pins used by the transceiver module
#define ss 16
#define rst 14
#define dio0 26

#define ledPin 18

void sendData(void * parameter) {
  for (;;) {
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    Serial.print("Sending packet: ");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" C, ");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Send LoRa packet to receiver
    LoRa.beginPacket();
    LoRa.print(temperature);
    LoRa.print(" C ");
    LoRa.print(humidity);
    LoRa.print(" %");
    LoRa.endPacket();

    vTaskDelay(10000 / portTICK_PERIOD_MS);  // Delay for 10 seconds
  }
}

void receiveCommands(void * parameter) {
  for (;;) {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      String receivedCommand = "";
      while (LoRa.available()) {
        receivedCommand += (char)LoRa.read();
      }
      Serial.print("Received command: ");
      Serial.println(receivedCommand);

      if (receivedCommand == "LED ON") {
        digitalWrite(ledPin, HIGH);
        Serial.println("LED turned ON");
      } else if (receivedCommand == "LED OFF") {
        digitalWrite(ledPin, LOW);
        Serial.println("LED turned OFF");
      }
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);  // Delay for 100 milliseconds
  }
}

void setup() {
  //initialize Serial Monitor
  dht.begin();
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);

  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(866E6)) {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");

  // Create FreeRTOS tasks
  xTaskCreate(sendData, "SendData", 1024 * 4, NULL, 1, NULL);
  xTaskCreate(receiveCommands, "ReceiveCommands", 1024 * 4, NULL, 1, NULL);
}

void loop() {
  // FreeRTOS tasks will handle the functionality
}