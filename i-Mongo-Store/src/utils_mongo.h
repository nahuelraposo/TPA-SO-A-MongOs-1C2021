#ifndef I_MONGO_STORE_SRC_UTILS_MONGO_H_
#define I_MONGO_STORE_SRC_UTILS_MONGO_H_

#include "../../utils/utils/utils.h"
#include "filesystem.h"
#include "sabotaje.h"
#include "msg_Mongo.h"

char* ip_Mongo_Store;
char* puerto_escucha;

char* ip_Mi_RAM_HQ;
char* puerto_Mi_RAM_HQ;
uint32_t socket_Mi_RAM_HQ;

char* ip_discordiador;
char* puerto_discordiador;
int socket_discordiador;

char* punto_montaje;
char* archivo_log;
char* bitarrayFS;

uint32_t tiempo_sincronizacion;
t_list* posiciones_sabotaje;
uint32_t proxima_posicion_sabotaje;
t_posicion* posicion_sabotaje;

t_config* config;
t_log* logger;
pthread_t hiloReceive;
pthread_mutex_t mutex_bitmap;
pthread_mutex_t mutex_hash;

uint32_t blocks;
uint32_t block_size;
t_bitarray* bitmap;
void* contenido_blocks;
void* contenido_blocks_aux;
pthread_mutex_t mutex_FS;
pthread_mutex_t mutex_Tareas;

char* path_files;
char* path_blocks;
char* path_superbloque;
char* path_bitacoras;
char* bitarray_string;

void arrancar_servidor(void);
void serve_client(int*);
void procesar_mensaje_recibido(int, int);
void verificar_archivo_tarea(t_tarea_Mongo*);
void procesar_tarea(t_tarea_Mongo*);
void informar_falta_archivo(t_tarea_Mongo*, char*);
void procesar_falta_archivo(t_tarea_Mongo*,char*);
uint32_t cantidad_caracteres_archivo(char, char*);
void eliminar_caracteres_llenado_a_archivo(char, uint32_t, char*);
void eliminar_caracteres_llenado_segun_tarea(char*, uint32_t );
void agregar_caracteres_llenado_a_archivo(char, uint32_t, char*);
void agregar_caracteres_llenado_segun_tarea(char*, uint32_t);
char obtener_caracter_llenado(char*);
void subir_a_bitacora(char*,uint32_t);

#endif /* I_MONGO_STORE_SRC_UTILS_MONGO_H_ */
