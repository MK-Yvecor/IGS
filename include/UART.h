#include <Arduino.h>

class UARTSender
{
    private:
        char GetType(int)    { return 'i'; }
        char GetType(double) { return 'd'; }
    

        byte *frame;
        byte *buffer;

        size_t offset = 0;


        template <typename input>
        void MakeFrame(input value, char *type){
            frame = &buffer[offset];

            char dataType = GetType(value);
            byte* b = (byte*)(void*)&value;
            size_t size;

            //building frame

            size = sizeof(value);

            frame[0] = 0xFF;
            frame[1] = type[0];
            frame[2] = type[1];
            frame[3] = dataType;
            frame[4] = (byte)size;

            for(int i = 0; i<(int)size; i++) {
                frame[5+i] = b[i];
            }

            frame[5+(int)size] = 0xF0;

            offset += 6 + size;
        }

    public:
    void AllocateMemoryForBuffer(){

        buffer = (byte *)malloc(1024);

        if(buffer == nullptr){
            buffer = (byte *)ps_malloc(1024);
        }
    }

    template <typename input>   
    void WriteToBuffer(input value, char *type){
        MakeFrame(value, type);
    }

    byte* GetBuffer(){
        byte *getbuffer = buffer;

        return getbuffer;
    }

     size_t GetBufferSize(){
        size_t getbuffersize = offset;

        return getbuffersize;
    }

    void SetBufferSize(size_t size){
        offset = size;
    }

    void SendBuffer(){
        Serial.write(buffer, offset);
        offset = 0;
    }


    template <typename input>
    void UartSend(input value, char type[2]) {
        char dataType = GetType(value);
        byte* b = (byte*)(void*)&value;
        size_t size = 2;
        char* buff = type;
        Serial.write(0xFF);
        Serial.write(buff, size);
        Serial.write(dataType);
        Serial.write(byte(sizeof(value)));
        Serial.write(b, sizeof(value));
        Serial.write(0xF0);
    }

};

class UARTReader{
    public:
        void UartParse(byte *b, char *command, float *value, bool* cycle_end);

    private:

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
    byte data_arr[4];

    uint8_t command_index = 0;
    uint8_t data_index = 0;
    uint8_t data_length = 0;


    void resetParser();


};