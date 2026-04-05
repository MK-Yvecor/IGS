#include <Arduino.h>
#include <RadioLib.h>
#include <Adafruit_NeoPixel.h>
#include <TinyGPS++.h>
#include <string.h>
#include <EEPROM.h>
#include "UART.h"
#include "Commands.h"

#define L_CS 8
#define L_DIO0 9
#define L_RST 4

#define SCK 5
#define MISO 6
#define MOSI 7

#define LED_COUNT 1
#define LED_PIN 1

const int resetPin = 10;
const int rxPin = 18; // TX z L86
const int txPin = 17; // RX z L86

TinyGPSPlus gps;
float PSRAM_SIZE;
float FLASH_SIZE;
String msg;
double test = 134.456;

SPIClass spiLoRa(FSPI);
Adafruit_NeoPixel WS2812B(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
SX1278 radio = new Module(L_CS, L_DIO0, L_RST, -1, spiLoRa);

Commands comm;
UARTReader UR;
UARTSender US;

char command[6];
float value;
byte b;
bool parser_status;
size_t len;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  WS2812B.begin();
  WS2812B.setPixelColor(0,0,10,0);
  WS2812B.show();
  US.AllocateMemoryForBuffer();
  Serial1.begin(9600, SERIAL_8N1, rxPin, txPin);

  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, LOW);
  delay(500);
  digitalWrite(resetPin, HIGH);
  delay(5000);

  spiLoRa.begin(SCK, MISO, MOSI, L_CS);
  int state = radio.begin(433.96, 125.0, 9, 7, 0x12, 17);
    if(state != RADIOLIB_ERR_NONE){
    WS2812B.setPixelColor(0,50,0,0);
    Serial.print("Failed to initialize LoRa, ERR Code: ");
    Serial.println(state);
    delay(3000);
   // return;
  }
  else { 
    Serial.println("Radio Ready"); 
    for(int i = 0; i<=5; i++){
      delay(250);
      WS2812B.setPixelColor(0,50,50,0);
      WS2812B.show();
      delay(250);
      WS2812B.setPixelColor(0,0,0,0);
      WS2812B.show();
    }
    WS2812B.setPixelColor(0,0,50,0);
    WS2812B.show();
  }

}



void wyswietlStatystyki() {
  Serial.print(F("Lokalizacja: ")); 
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Satelity: "));
  Serial.println(gps.satellites.value());
}

void loop() {
  /*while (Serial.available() > 0) {
    b = Serial.read();
    UR.UartParse(&b, command, &value, &parser_status);
    if(parser_status)
    comm.HandleCommand((char *)command, value);
  }*/


  len = radio.getPacketLength();
  int state = radio.receive(US.GetBuffer(), len);
  US.SetBufferSize(len);
  US.SendBuffer();

  /*if(state == RADIOLIB_ERR_NONE){
    Serial.print("received: ");
    Serial.println(msg);
    Serial.println("===================");
    Serial.print("RSSI: ");
    Serial.print(radio.getRSSI());
    Serial.println("dBm");
    Serial.print("SNR: ");
    Serial.print(radio.getSNR());
    Serial.println("dB");
    Serial.println();
  }
  else{
    Serial.println("RX Failed");
  }*/
  
  /*while (Serial1.available() > 0) {
    char c = Serial1.read();
    // Serial.print(c); 
    
    if (gps.encode(c)) {
      wyswietlStatystyki();
    }
  }


  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 5000) {
    if (gps.charsProcessed() < 10) {
      Serial.println("BŁĄD: Serial1 żyje, ale nie widzę ramek NMEA. Sprawdź RX/TX!");
    }
    lastCheck = millis();
}*/
}