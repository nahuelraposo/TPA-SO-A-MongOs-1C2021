#ifndef DISCORDIADOR_SRC_DISCORDIADOR_H_
#define DISCORDIADOR_SRC_DISCORDIADOR_H_

#include "utils_discordiador.h"
#include "consola_discordiador.h"
#include "planificacion.h"

#define ARCHIVO_CONFIG "../discordiador.config"

void iniciar_discordiador(void);
void leer_config(void);
void iniciar_patotas(void);
void finalizar_discordiador(void);
void conectar_modulos(void);

#endif /* DISCORDIADOR_SRC_DISCORDIADOR_H_ */
