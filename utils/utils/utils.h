#ifndef SHARED_HELLO_H_
#define SHARED_HELLO_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/bitarray.h>
#include <commons/txt.h>
#include <commons/process.h>
#include <commons/memory.h>
#include <readline/readline.h>
#include <commons/temporal.h>
#include <readline/readline.h>
#include <semaphore.h>
#include "buffer.h"
#include "msgtypes.h"
#include "paquete.h"
#include "sockets.h"
#include "nivel-gui/tad_nivel.h"
#include "nivel-gui/nivel-gui.h"


t_log* iniciar_logger(char*, char*);
int contar_elementos_array(char**);
uint32_t minimo(uint32_t numero1, uint32_t numero2);
int son_iguales(char*, char*);
int chequear_argumentos_del_mensaje(char**, int);
int existe_en_array(char**, char*);
t_posicion obtener_posicion_origen(t_posicion* posicion, t_movimiento direccion);

void helloWorld(void);

#endif
