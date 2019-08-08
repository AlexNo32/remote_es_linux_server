//
// Created by charname on 7/23/19.
//
#include <stdio.h>
#include <stdlib.h>
#include "stringbuffer.h"

void vector_init(Vector *vector) {
    // initialize size and capacity
    vector->size = 0;
    vector->capacity = VECTOR_INITIAL_CAPACITY;

    // allocate memory for vector->data
    vector->data = malloc(sizeof(int) * vector->capacity);
}

void vector_append(Vector *vector, char* value, size_t nCount) {
    // make sure there's room to expand into
    while (nCount > (vector->capacity - vector->size)){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, sizeof(char) * vector->capacity);
    }
    vector->size += snprintf(vector->data + vector->size, nCount + 1, "%s", value);
}

int vector_get(Vector *vector, int index) {
    if (index >= vector->size || index < 0) {
        printf("Index %d out of bounds for vector of size %d\n", index, vector->size);
        exit(1);
    }
    return vector->data[index];
}

void vector_free(Vector *vector) {
    free(vector->data);
}
