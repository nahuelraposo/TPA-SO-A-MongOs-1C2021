#ifndef MI_RAM_HQ_SRC_MSG_RAM_H_
#define MI_RAM_HQ_SRC_MSG_RAM_H_

#include "utils_ram.h"

void enviar_solicitar_tarea_respuesta(t_tarea* tarea, uint32_t socket_cliente);
void enviar_respuesta_listado_tripulantes(t_list*,uint32_t);

t_buffer* serializar_memoria_tareas(char* tareas);
t_buffer* serializar_memoria_pcb(t_pcb* pcb);
t_buffer* serializar_memoria_tcb(t_tcb* tcb);
void serializar_solicitar_tarea_respuesta(t_tarea* msg, t_buffer* buffer);
void serializar_respuesta_listado_tripulantes(t_list*, t_buffer*);

t_tripulante* deserializar_expulsar_tripulante(uint32_t socket_cliente);
t_tripulante* deserializar_solicitar_tarea(uint32_t socket_cliente);
t_movimiento deserializar_mover_hacia_direccion(uint32_t socket_cliente);
t_tcb* deserializar_memoria_tcb(void* stream);
t_pcb* deserializar_memoria_pcb(void* stream);

#endif /* MI_RAM_HQ_SRC_MSG_RAM_H_ */
