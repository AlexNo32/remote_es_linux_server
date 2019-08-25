/*
 ============================================================================
 Name        : Simple Remote Execution System Linux client
 Author      : Alex
 Version     : 0.9v
 Copyright   : No copyright
 Description : 2803ICT assignment 1, Ansi-style, CLion + Ubuntu
 ============================================================================
 */

#ifndef STRINGBUFFERTEST_STRINGBUFFER_H
#define STRINGBUFFERTEST_STRINGBUFFER_H

#define BUFFER_INITIAL_CAPACITY 1024

typedef struct {
    int size;      // slots used so far
    int capacity;  // total available slots
    char *data;     // array of integers we're storing
} Buffer;

void buffer_init(Buffer *buffer);

void buffer_append(Buffer *buffer, char* value, size_t nCount);

void buffer_append_short(Buffer *buf, int value);

void buffer_append_int(Buffer *buf, int value);

void buffer_append_timestamp(Buffer *buf, long long value);

void buffer_free(Buffer *vector);

#endif //STRINGBUFFERTEST_STRINGBUFFER_H
