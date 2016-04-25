#ifndef _APPLICATION_H_
#define _APPLICATION_H_

enum command_t {
    PUT = 1,
    GET,
    DELETE
};

enum return_code {
    SUCCESS,
    GOT_VALUE,
    NOT_FOUND,
    INVALID_OP,
    FAILED
};


#endif