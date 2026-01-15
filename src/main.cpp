#include <Arduino.h>

float PSRAM_SIZE;
float FLASH_SIZE;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  PSRAM_SIZE = ESP.getPsramSize();
  FLASH_SIZE = ESP.getFlashChipSize();

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("PSRAM SIZE [KB]: ");
  Serial.println(PSRAM_SIZE / 1024);
  Serial.print("PSRAM SIZE [MB]: ");
  Serial.println(PSRAM_SIZE / 1024 / 1024);

  Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=");

  Serial.print("FLASH SIZE [KB]: ");
  Serial.println(FLASH_SIZE / 1024);
  Serial.print("FLASH SIZE [MB]: ");
  Serial.println(FLASH_SIZE / 1024 / 1024);

  delay(1000);
}
