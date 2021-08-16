#ifndef DISCORDIADOR_SRC_MSG_DISCORDIADOR_H_
#define DISCORDIADOR_SRC_MSG_DISCORDIADOR_H_

#include "utils_discordiador.h"

//---------ENVIO DE MENSAJES--------//
void enviar_iniciar_patota(t_patota*, uint32_t);
void enviar_iniciar_tripulante(t_tripulante*, uint32_t);
void enviar_RAM_expulsar_tripulante(t_tripulante* tripulante, uint32_t);
void enviar_RAM_listar_tripulantes(t_tripulante*, uint32_t);
void enviar_RAM_actualizar_estado(t_tripulante* tripulante, uint32_t socket_conexion);
void enviar_mover_hacia(t_tripulante* tripulante, t_movimiento direccion);
void enviar_Mongo_bitacora_tarea(t_tripulante* msg, uint32_t socket_conexion);
void enviar_solicitar_tarea(t_tripulante* tripulante, uint32_t socket_conexion);
void enviar_Mongo_obtener_bitacora(uint32_t id_tripulante, uint32_t socket_conexion);
void enviar_Mongo_bitacora_tarea_finalizar(t_tripulante* tripulante, uint32_t socket_conexion);
void enviar_Mongo_tarea_e_s(t_tripulante* tripulante, uint32_t socket_conexion);

//---------SERIALIZACION--------//
void serializar_expulsar_tripulante(uint32_t, t_buffer*);
void serializar_iniciar_patota(t_patota*, t_buffer*);
void serializar_iniciar_tripulante(t_tripulante*, t_buffer*);
void serializar_listar_tripulantes(t_tripulante*, t_buffer*);
void serializar_mover_hacia(t_tripulante* tripulante,t_movimiento direccion, t_buffer* buffer);
void serializar_bitacora_tarea(t_tripulante* msg, t_buffer* buffer);
void serializar_mover_hacia_Mongo(t_tripulante* tripulante,t_movimiento direccion, t_buffer* buffer);
void serializar_ids_tripulante(t_tripulante* msg, t_buffer* buffer);
void serializar_actualizar_estado(t_tripulante* msg, t_buffer* buffer);
void serializar_tarea_e_s(t_tripulante* msg, t_buffer* buffer);

//---------DESERIALIZACION--------//
void listar_tripulantes_respuesta(uint32_t socket_cliente);	// TODO ??
t_tarea* recibir_tarea(uint32_t socket_cliente);
t_tarea* deserializar_solicitar_tarea_respuesta(uint32_t socket_cliente);
t_posicion* deserializar_posicion_sabotaje(uint32_t socket_cliente);
t_list* deserializar_respuesta_listado_tripulantes(uint32_t socket_cliente);

t_list* recibir_listado_tripulantes(int socket_cliente);
char* recibir_obtener_bitacora_respuesta(int socket_cliente);
#endif /* DISCORDIADOR_SRC_MSG_DISCORDIADOR_H_ */
