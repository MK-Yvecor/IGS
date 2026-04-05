#include "UART.h"

void UARTReader::UartParse(byte *b, char *command, float *value, bool* cycle_end) {
     if (*b == 0xFF) {
    _parserstate = CommandRead;
    command_index = 0;
    data_index = 0;
    data_length = 0;
    *cycle_end = false;
    return;
  }

  switch (_parserstate) {

    case Idle:
      break;

      case CommandRead:
      command[command_index++] = *b;

          if (command_index == 5) { 
            command[5] = '\0';
            _parserstate = DataLength;
          }
      break;

    case DataLength:
      data_length = *b;

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
      data_arr[data_index] = *b;
      data_index++;

      if (data_index >= data_length) {
        _parserstate = EndByte;
      }
      break;

    case EndByte:
      if (*b == 0xF0) {
        memcpy(value, data_arr, 4);
        *cycle_end = true;
      }

      resetParser();
      break;
  }
}

void UARTReader::resetParser(){
    command_index = 0;
    data_index = 0;
    data_length = 0;
}