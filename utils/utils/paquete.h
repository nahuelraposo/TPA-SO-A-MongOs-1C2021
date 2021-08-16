#ifndef SHARED_SHARED_PAQUETE_H_
#define SHARED_SHARED_PAQUETE_H_

#include "sockets.h"
#include "buffer.h"
#include <commons/collections/list.h>
#include <commons/string.h>

//---------MENSAJES--------//
typedef enum {
	MSGTYPE_UNKNOWN = 0,
	INICIAR_PLANIFICACION,
	PAUSAR_PLANIFICACION,
	INICIAR_PATOTA,
	INICIAR_TRIPULANTE,
	LISTAR_TRIPULANTES,
	RESPUESTA_LISTAR_TRIPULANTES,
	EXPULSAR_TRIPULANTE,
	OBTENER_BITACORA,
	OBTENER_BITACORA_RESPUESTA,
	ESTA_ON,
	SABOTAJE,
	ARRANCAR_TAREAS,
	MOVER_HACIA,
	SOLICITAR_TAREA,
	EJECUTAR_TAREA,
	FINALIZAR_TAREA,
	RESOLVER_SABOTAJE,
	FINALIZAR_SABOTAJE,
	TAREA_E_S,
	ACTUALIZAR_ESTADO_TRIPULANTE,
	TRIPULANTE_SABOTAJE,
	RESPUESTA
}op_code;


typedef struct {
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

t_paquete* crear_paquete(op_code codigo_operacion);
void agregar_a_paquete(t_paquete* paquete, char* valor);
t_buffer* serializar_paquete(t_paquete* paquete);
void eliminar_paquete(t_paquete* paquete);
void enviar_paquete(t_paquete* paquete, int socket_cliente);

#endif /* SHARED_SHARED_PAQUETE_H_ */
