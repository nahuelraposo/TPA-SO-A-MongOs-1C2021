#include "buffer.h"

t_buffer* crear_buffer()
{
    t_buffer* unBuffer = malloc(sizeof(t_buffer));
    unBuffer->size = 0;
    unBuffer->stream = NULL;
    return unBuffer;
}

void agregar_a_buffer(t_buffer* buffer, void* src, int size)
{
    buffer->stream = realloc(buffer->stream, buffer->size + size);
    memcpy(buffer->stream + buffer->size, src, size);
    buffer->size += size;
}

void quitar_de_buffer(t_buffer* buffer, void* dest, int size)
{
    memcpy(dest, buffer->stream, size);
    buffer->size -= size;
    memmove(buffer->stream, buffer->stream + size, buffer->size);
    buffer->stream = realloc(buffer->stream, buffer->size);
}

void eliminar_buffer(t_buffer* buffer)
{
    if(buffer->size > 0)
    {
        free(buffer->stream);
    }
    free(buffer);
}

