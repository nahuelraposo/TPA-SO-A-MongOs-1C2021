#ifndef DISCORDIADOR_SRC_TRIPULANTES_H_
#define DISCORDIADOR_SRC_TRIPULANTES_H_

#include "utils_discordiador.h"
#include "msg_discordiador.h"

t_tripulante* obtener_tripulante_de_patota(t_patota* patota, int i);
void ejecutar_tripulante(t_tripulante* tripulante);
void actualizar_estado(t_tripulante* tripulante);
bool verificar_estado(t_tripulante* tripulante);
bool puedo_seguir(p_tripulante* tripulante_plani);
void hacer_tarea(p_tripulante* tripulante);
bool es_tarea_IO(char* tarea);
void hacer_ciclos_tarea(t_tripulante* tripulante);
bool quedan_pasos(t_tripulante* tripulante);
bool esta_en_el_lugar(t_posicion* posicion1, t_posicion* posicion2);
bool quedan_movimientos(uint32_t posicion1, uint32_t posicion2);
t_movimiento avanzar_hacia(t_tripulante* tripulante, t_posicion* posicion_meta, bool es_sabotaje);
void hacer_peticon_IO(void);
bool es_tarea_de_recursos(char*);
bool es_tarea_de_eliminar_residuos(char*);
void subir_tripulante_ready(p_tripulante* tripulante_plani);
bool existe_archivo(char*);

#endif /* DISCORDIADOR_SRC_TRIPULANTES_H_ */
