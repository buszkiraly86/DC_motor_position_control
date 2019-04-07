typedef struct {
    const char* command_name;
    void (*command_fn)(const unsigned char argc, const char** argv);
} command;

int comms_initialize();
int comms_process();
int comms_add_command(const char* cmd);

