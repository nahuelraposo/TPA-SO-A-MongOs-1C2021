#ifndef DISCORDIADOR_SRC_PLANIFICACION_H_
#define DISCORDIADOR_SRC_PLANIFICACION_H_

#include "utils_discordiador.h"
#include "tripulantes.h"

void preparar_planificacion(void);
void planificar_patota(t_patota*);
void arrancar_planificacion(void);
void iniciar_planificacion(void);
void planificar_tripulantes_bloqueados(void);
void pausar_planificacion(void);
void crear_colas_planificacion(void);
bool cola_bloqueados_vacia(void);
bool cola_ready_vacia(void);
bool verificar_planificacion_activa(void);
void planificacion_segun_FIFO(void);
void planificacion_segun_RR(void);
void finalizar_tripulante_plani(uint32_t);

#endif /* DISCORDIADOR_SRC_PLANIFICACION_H_ */
