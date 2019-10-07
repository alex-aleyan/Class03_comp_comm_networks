// if ack is unset, indicates lines to be tx'd or line tag of tx'd line, if ack set indicates the line the server is requesting
typedef struct file_x_app_layer{
    unsigned int file_id       : 16;
    unsigned int text_lines    : 16; 
    unsigned int payload_size  : 16;
    unsigned int tx_burst      : 1;
    unsigned int server_ack    : 1; 
    unsigned int reserved      : 14;
} file_x_app_layer_t;

