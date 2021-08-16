#ifndef SHARED_SHARED_BUFFER_H_
#define SHARED_SHARED_BUFFER_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct {
    int size;
    void* stream;
} t_buffer;

t_buffer* crear_buffer();
void agregar_a_buffer(t_buffer*, void*, int);
void quitar_de_buffer(t_buffer* buffer, void* dest, int size);
void eliminar_buffer(t_buffer* buffer);

#endif /* SHARED_SHARED_BUFFER_H_ */
