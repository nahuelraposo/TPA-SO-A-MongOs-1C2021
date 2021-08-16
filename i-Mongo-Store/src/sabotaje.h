#ifndef I_MONGO_STORE_SRC_SABOTAJE_H_
#define I_MONGO_STORE_SRC_SABOTAJE_H_

#include "utils_mongo.h"
#include "dirent.h"

void verificar_sabotajes(void);
void procesar_nuevo_sabotaje(int signal);
bool hay_sabotajes(void);
t_respuesta resolver_sabotaje();


char* obtener_path_bitacora(char* nombreBitacora);
FILE* abrirSuperbloque(char* modo);
t_list* obtener_bloques_usados();
t_list* obtener_bloques_bitacora();
t_list* obtener_bloques_recursos();

bool detectar_sabotaje_superbloque_bitmap();
void resolver_sabotaje_superbloque_bitmap(t_bitarray* bitarray, t_list* bloques_usados);
bool detectar_sabotaje_superbloque_blocks();
void resolver_sabotaje_superbloque_blocks();
bool detectar_sabotaje_files_blockcount(t_config*);
void resolver_sabotaje_files_blockcount(t_config* archivo_recurso, uint32_t cantidad_bloques);
void actualizar_valor_blocks_superbloque(uint32_t blocksReales);
t_list* listaArchivosDeBitacora();
char* obtener_path_bitacora(char* nombreBitacora);
bool chequear_sabotajes_en_recurso(char* path_relativo);
bool detectar_sabotaje_files_size(t_config*);
bool detectar_sabotaje_files_blocks();
void resolver_sabotaje_files_size(t_config*, uint32_t);
void resolver_sabotaje_files_blocks(t_config*);
bool detectar_algun_sabotaje_en_files();
bool detectar_algun_sabotaje_en_superbloque();
uint32_t tamanio_real_blocks_ims();

#endif /* I_MONGO_STORE_SRC_SABOTAJE_H_ */
