#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#include <time.h>

struct client_request {
    unsigned length;
    struct timespec ts;
    char content[1];
};

unsigned message_length(struct client_request *request);
struct client_request* create_client_request(const char *data, unsigned data_size);

#endif