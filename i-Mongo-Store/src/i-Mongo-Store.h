#ifndef I_MONGO_STORE_SRC_I_MONGO_STORE_H_
#define I_MONGO_STORE_SRC_I_MONGO_STORE_H_

#include "utils_mongo.h"
#include "consola_mongo.h"

#define ARCHIVO_CONFIG "../i-Mongo-Store.config"
void inicializar_filesystem(void);
void iniciar_mongo(void);
void leer_config(void);
void FS_RESET(void);
//void crear_punto_de_montaje(void);
void obtener_posiciones_sabotaje(void);
void finalizar_i_mongo(int signum);



#endif /* I_MONGO_STORE_SRC_I_MONGO_STORE_H_ */
