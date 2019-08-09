/*
 ============================================================================
 Name        : Simple Remote Execution System Linux server
 Author      : Alex
 Version     : 0.9v
 Copyright   : No copyright
 Description : 2803ICT assignment 1, Ansi-style, CLion + Ubuntu
 ============================================================================
 */

#ifndef STRINGBUFFERTEST_STRINGBUFFER_H
#define STRINGBUFFERTEST_STRINGBUFFER_H

#define VECTOR_INITIAL_CAPACITY 1024

typedef struct {
    int size;      // slots used so far
    int capacity;  // total available slots
    char *data;     // array of integers we're storing
} Vector;

void vector_init(Vector *vector);

void vector_append(Vector *vector, char* value, size_t nCount);

int vector_get(Vector *vector, int index);

//void vector_set(Vector *vector, int index, int value);

//void vector_double_capacity_if_full(Vector *vector);

void vector_free(Vector *vector);

#endif //STRINGBUFFERTEST_STRINGBUFFER_H
