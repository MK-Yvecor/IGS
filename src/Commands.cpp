#include <Arduino.h>
#include "Commands.h"

void Commands::cmd_setFreq(float value){
  Serial.println(value);
}

void Commands::cmd_R_IMU(float value){
  Serial.println(value);
}

void Commands::cmd_C_IMU(float value){
  Serial.println(value);
}

void Commands::HandleCommand(char* commandr, float value) {

  for(int i = 0; i<=1; i++){
      if(strcmp(commandr, commands[i].name) == 0){
        commands[i].func(value);
      }
  }
}