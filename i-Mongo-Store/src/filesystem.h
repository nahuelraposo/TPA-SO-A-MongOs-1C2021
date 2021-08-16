#ifndef I_MONGO_STORE_SRC_FILESYSTEM_H_
#define I_MONGO_STORE_SRC_FILESYSTEM_H_

#include "utils_mongo.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void sincronizar_blocks();
void inicializar_paths_aux();
void leer_superbloque(FILE*);
void crear_superbloque();
void obtener_superbloque();
void inicializar_bloques();
bool directorio_existe(char*);
bool archivo_existe(char*);
bool archivo_recursos_existe(char*);
char* obtener_path_files(char*);
int crear_archivo(char*);
int crear_archivo_recursos(char*, char);
int eliminar_archivo(char*);
void vaciar_archivo(char*);
void crear_blocks();
char* path_bitacora_tripulante(uint32_t);
void subir_FS(char*, char*, bool);
int crear_archivo_bitacora(uint32_t);
t_list* sacar_bloques_config(t_list* bloques, t_config* config);
t_list* sumar_bloques_config(t_list* bloques, t_config* config);
t_bitarray* leer_bitmap();
void subir_bitmap(t_bitarray* bitarray);
char* dar_hash_md5(char* archivo);
char* obtener_hashmd5_string(char* stream);
char* obtener_caracteres_de_file(t_config* file_recurso);
char* obtener_caracteres(t_list* bloques, char caracter_llenado);
char* obtener_caracteres2(t_list* bloques);


t_bitarray* crear_bitmap(void);
#endif /* I_MONGO_STORE_SRC_FILESYSTEM_H_ */
