class Commands
{
    public:
        void HandleCommand(char* commandr, float value);


    private:
        typedef void (*CommandCallback)(float value);

        struct Command {
            const char* name;
            CommandCallback func;
        };


        static void cmd_setFreq(float value);

        static void cmd_R_IMU(float value);

        static void cmd_C_IMU(float value);


        static inline Command commands[] = {
            {"LoRaF",  cmd_setFreq},
            {"R_IMU", cmd_R_IMU},
            {"C_IMU", cmd_C_IMU},
        };


};

