#include <Arduino.h>
#include <RadioLib.h>
#include <Adafruit_NeoPixel.h>
#include <TinyGPS++.h>
#include <string.h>

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

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  WS2812B.begin();
  WS2812B.setPixelColor(0,0,10,0);
  WS2812B.show();
  Serial1.begin(9600, SERIAL_8N1, rxPin, txPin);

  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, LOW);
  delay(500);
  digitalWrite(resetPin, HIGH);
  delay(5000);

  spiLoRa.begin(SCK, MISO, MOSI, L_CS);
  int state = radio.begin(433.0, 125.0, 9, 7, 0x12, 17);
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

template <typename T> char GetType();
template<> char GetType<int>() { return 'i'; }
template<> char GetType<double>() { return 'd'; }

template <typename input>
void UartSend(input value){
  char dataType = GetType<decltype(value)>();
  byte* b = (byte*)(void*)&value;
  Serial.write(0xFF);
  Serial.write("TE");
  Serial.write(dataType);
  Serial.write(byte(sizeof(value)));
  Serial.write(b, sizeof(value));
  Serial.write(0xF0);
}


typedef void (*CommandCallback)(float value);

struct Command {
    const char* name;
    CommandCallback func;
};


void cmd_setFreq(float value){
  Serial.println(value);
}

void cmd_R_IMU(float value){
  Serial.println(value);
}

void cmd_C_IMU(float value){
  Serial.println(value);
}


Command commands[] = {
    {"LoRaF",  cmd_setFreq},
    {"R_IMU", cmd_R_IMU},
    {"C_IMU", cmd_C_IMU},
};


void HandleCommand(char* commandr, float value) {

  for(int i = 0; i<=1; i++){
      if(strcmp(commandr, commands[i].name)== 0){
        commands[i].func(value);
      }
  }
}

#define CMD_LEN 6
#define MAX_DATA_LEN 64 


enum ParserState {
  Idle,
  CommandRead,
  DataLength,
  Data,
  EndByte
};

ParserState _parserstate = Idle;


byte command[CMD_LEN];
byte data_arr[MAX_DATA_LEN];

uint8_t command_index = 0;
uint8_t data_index = 0;
uint8_t data_length = 0;


void resetParser() {
  command_index = 0;
  data_index = 0;
  data_length = 0;
  _parserstate = Idle;
}

void UartParse(byte b) {

  if (b == 0xFF) {
    _parserstate = CommandRead;
    command_index = 0;
    data_index = 0;
    data_length = 0;
    return;
  }

  switch (_parserstate) {

    case Idle:
      break;

      case CommandRead:
      command[command_index++] = b;

          if (command_index == 5) { 
            command[5] = '\0';
            _parserstate = DataLength;
          }
      break;

    case DataLength:
      data_length = b;

      if (data_length > MAX_DATA_LEN) {
        resetParser();
        break;
      }

      if (data_length == 0) {
        _parserstate = EndByte;
      } else {
        data_index = 0;
        _parserstate = Data;
      }
      break;

    case Data:
      data_arr[data_index++] = b;

      if (data_index >= data_length) {
        _parserstate = EndByte;
      }
      break;

    case EndByte:
      if (b == 0xF0) {
        HandleCommand((char*)command, 100.2f);
      }

      resetParser();
      break;
  }
}

void loop() {
  while (Serial.available() > 0) {
    byte b = Serial.read();
    UartParse(b);
  }



  int state = radio.receive(msg);
  if(state == RADIOLIB_ERR_NONE){
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
  }
  
  while (Serial1.available() > 0) {
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
}
}