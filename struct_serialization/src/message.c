#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "message.h"

unsigned message_length(struct client_request *request)
{
    return request->length - (sizeof(struct client_request) - 1);
}

struct client_request* create_client_request(const char *data, unsigned data_size)
{
    unsigned message_size = sizeof(struct client_request) + data_size - 1;
    struct client_request *request = (struct client_request*)malloc(message_size);
    request->length = message_size;
    memcpy(request->content, data, data_size);
    return request;
}