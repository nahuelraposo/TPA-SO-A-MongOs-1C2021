#ifndef DISCORDIADOR_SRC_CONSOLA_DISCORDIADOR_H_
#define DISCORDIADOR_SRC_CONSOLA_DISCORDIADOR_H_

#include "utils_discordiador.h"
#include "msg_discordiador.h"
#include "planificacion.h"

void leer_consola(void);
bool verifica_path_tareas(char* path_tareas);
char* de_listado_a_string(t_list* lista_tripulantes);
char* estado_string(char estado);
void procesar_mensajes_en_consola_discordiador(char**);
t_patota* de_consola_a_patota(char**);

#endif /* DISCORDIADOR_SRC_CONSOLA_DISCORDIADOR_H_ */
