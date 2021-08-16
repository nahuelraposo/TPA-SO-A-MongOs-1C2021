#include "msg_discordiador.h"

void enviar_iniciar_patota(t_patota* msg, uint32_t socket_conexion){
	t_paquete* paquete_a_enviar = crear_paquete(INICIAR_PATOTA);
	serializar_iniciar_patota(msg, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}
void enviar_iniciar_tripulante(t_tripulante* tripulante, uint32_t socket_conexion){
	t_paquete* paquete_a_enviar = crear_paquete(INICIAR_TRIPULANTE);
	serializar_iniciar_tripulante(tripulante, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_solicitar_tarea(t_tripulante* tripulante, uint32_t socket_conexion){
	t_paquete* paquete_a_enviar = crear_paquete(SOLICITAR_TAREA);
	serializar_ids_tripulante(tripulante, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_RAM_listar_tripulantes(t_tripulante* tripulante, uint32_t socket_conexion){
	t_paquete* paquete_a_enviar = crear_paquete(LISTAR_TRIPULANTES);
	serializar_listar_tripulantes(tripulante, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_RAM_actualizar_estado(t_tripulante* tripulante, uint32_t socket_conexion){
	t_paquete* paquete_a_enviar = crear_paquete(ACTUALIZAR_ESTADO_TRIPULANTE);
	serializar_actualizar_estado(tripulante, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_RAM_expulsar_tripulante(t_tripulante* tripulante, uint32_t socket_conexion) {
	t_paquete* paquete_a_enviar = crear_paquete(EXPULSAR_TRIPULANTE);
	serializar_ids_tripulante(tripulante, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_Mongo_obtener_bitacora(uint32_t id_tripulante, uint32_t socket_conexion) {
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_BITACORA);

	paquete_a_enviar->buffer->size = sizeof(uint32_t);
	paquete_a_enviar->buffer->stream = malloc(paquete_a_enviar->buffer->size);
	memcpy(paquete_a_enviar->buffer->stream , &(id_tripulante), sizeof(uint32_t));

	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_Mongo_tripulante_sabotaje(t_tripulante* tripulante, int conexion_Mongo){
	t_paquete* paquete_a_enviar = crear_paquete(TRIPULANTE_SABOTAJE);
	serializar_tripulante_sabotaje(tripulante, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, conexion_Mongo);
}

char* deserializar_respuesta_obtener_bitacora_respuesta(uint32_t socket_cliente) {
	//------------ORDEN------------
	//1. Tam bitacora
	//2. Bitacora
	//-----------------------------

	uint32_t tamanio_bitacora;

	recv(socket_cliente, &(tamanio_bitacora), sizeof(uint32_t), 0);
	char* bitacora = malloc(tamanio_bitacora);
	recv(socket_cliente, bitacora, tamanio_bitacora, 0);

	return bitacora;
}

char* recibir_obtener_bitacora_respuesta(int socket_cliente){
	int aux;
	recv(socket_cliente, &aux, sizeof(op_code), MSG_WAITALL);
	recv(socket_cliente, &aux, sizeof(uint32_t), MSG_WAITALL);

	return deserializar_respuesta_obtener_bitacora_respuesta(socket_cliente);
}

void enviar_Mongo_bitacora_tarea(t_tripulante* tripulante, uint32_t socket_conexion) {
	t_paquete* paquete_a_enviar = crear_paquete(EJECUTAR_TAREA);
	serializar_bitacora_tarea(tripulante, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_Mongo_tarea_e_s(t_tripulante* tripulante, uint32_t socket_conexion) {
	t_paquete* paquete_a_enviar = crear_paquete(TAREA_E_S);
	serializar_tarea_e_s(tripulante, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_Mongo_bitacora_tarea_finalizar(t_tripulante* tripulante, uint32_t socket_conexion) {
	t_paquete* paquete_a_enviar = crear_paquete(FINALIZAR_TAREA);
	serializar_bitacora_tarea(tripulante, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_conexion);
}

void enviar_mover_hacia(t_tripulante* tripulante, t_movimiento direccion){
//	RAM
	t_paquete* paquete_a_enviar = crear_paquete(MOVER_HACIA);
	serializar_mover_hacia(tripulante, direccion, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, tripulante->socket_conexion_RAM);
//	MONGO
	paquete_a_enviar = crear_paquete(MOVER_HACIA);
	serializar_mover_hacia_Mongo(tripulante, direccion, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, tripulante->socket_conexion_Mongo);

}

void serializar_iniciar_patota(t_patota* msg, t_buffer* buffer){
	//------------ORDEN------------
	//1. Cantidad tripulantes
	//2. Tamaño del path
	//3. Path tareas
	//4. Posiciones tripulantes 	} COMO ES LISTA IRA DENTRO DE UN FOR
	//5. Id patota
	//-----------------------------

	uint32_t offset = 0;
	msg->tam_path = string_length(msg->path_tareas)+1;

	buffer->size = sizeof(uint32_t)*3;
	buffer->size += msg->tam_path;
	buffer->size += (2 * sizeof(uint32_t)) * msg->cant_tripulantes;

	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->cant_tripulantes), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->tam_path), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, msg->path_tareas, msg->tam_path);
	offset += msg->tam_path;

	for (int i = 0; i < msg->cant_tripulantes; i++) {
		t_posicion* posicion = (t_posicion*) list_get(msg->posiciones,i);

		memcpy(buffer->stream + offset, &(posicion->pos_x), sizeof(uint32_t));
		offset += sizeof(uint32_t);

		memcpy(buffer->stream + offset, &(posicion->pos_y), sizeof(uint32_t));
		offset += sizeof(uint32_t);
	}

	memcpy(buffer->stream + offset, &(msg->id_patota), sizeof(uint32_t));
}

void serializar_iniciar_tripulante(t_tripulante* msg, t_buffer* buffer){
	//------------ORDEN------------
	//1. ID
	//2. Patota asociada
	//3. Posicion X
	//4. Posicion Y
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t)*4;
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->id_patota_asociado), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->posicion->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(buffer->stream + offset, &(msg->posicion->pos_y), sizeof(uint32_t));
}

void serializar_actualizar_estado(t_tripulante* msg, t_buffer* buffer){
	//------------ORDEN------------
	//1. ID
	//2. Patota asociada
	//3. Estado
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t)*2 + sizeof(t_estado);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->id_patota_asociado), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->estado), sizeof(t_estado));
}

void serializar_bitacora_tarea(t_tripulante* msg, t_buffer* buffer) {
	//------------ORDEN------------
	//1. ID
	//2. Tamanio
	//3. Tarea
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = 2*sizeof(uint32_t) + msg->tarea_act->tamanio_tarea;
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->tarea_act->tamanio_tarea), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, msg->tarea_act->tarea, msg->tarea_act->tamanio_tarea);
}

void serializar_tarea_e_s(t_tripulante* msg, t_buffer* buffer) {
	//------------ORDEN------------
	//1. ID
	//2. Tamanio
	//3. Tarea
	//4. Parametro
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = 3*sizeof(uint32_t) + msg->tarea_act->tamanio_tarea;
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->tarea_act->tamanio_tarea), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, msg->tarea_act->tarea, msg->tarea_act->tamanio_tarea);
	offset += msg->tarea_act->tamanio_tarea;

	memcpy(buffer->stream + offset, &(msg->tarea_act->parametro), sizeof(uint32_t));
}

void serializar_mover_hacia(t_tripulante* tripulante, t_movimiento direccion, t_buffer* buffer){
	//------------ORDEN------------
	//1. ID
	//2. Direccion
	//3. ID Patota Asociada
	//4. Posicion origen
	//5. Posicion destino
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = 6*sizeof(uint32_t) + sizeof(t_movimiento);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(tripulante->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(direccion), sizeof(t_movimiento));
	offset += sizeof(t_movimiento);

	memcpy(buffer->stream + offset, &(tripulante->id_patota_asociado), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	t_posicion posicion_origen = obtener_posicion_origen(tripulante->posicion, direccion);
	memcpy(buffer->stream + offset, &(posicion_origen.pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(posicion_origen.pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(tripulante->posicion->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(tripulante->posicion->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
}

void serializar_mover_hacia_Mongo(t_tripulante* tripulante,t_movimiento direccion, t_buffer* buffer){
	//------------ORDEN------------
	//1. ID
	//2. Posicion origen
	//3. Posicion destino
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t)*5;
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(tripulante->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	t_posicion posicion_origen = obtener_posicion_origen(tripulante->posicion, direccion);
	memcpy(buffer->stream + offset, &(posicion_origen.pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(posicion_origen.pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(tripulante->posicion->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(tripulante->posicion->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
}

void serializar_listar_tripulantes(t_tripulante* msg, t_buffer* buffer) {
	//------------ORDEN------------
	//1. Id del tripulante
	//2. Id de patota
	//3. Estado
	//-----------------------------

	uint32_t offset = 0;

	buffer->size = sizeof(uint32_t)*3;
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->id_patota_asociado), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->estado), sizeof(uint32_t));
	offset += sizeof(uint32_t);
}

void serializar_ids_tripulante(t_tripulante* msg, t_buffer* buffer){
	//------------ORDEN------------
	//1. Id tripulante
	//2. Id Patota asociada
	//-----------------------------

	uint32_t offset = 0;

	buffer->size = 2 * sizeof(uint32_t);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(msg->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(msg->id_patota_asociado), sizeof(uint32_t));
}

void serializar_tripulante_sabotaje(t_tripulante* tripulante, t_buffer* buffer){
	//------------ORDEN------------
	//1. ID
	//2. Patota asociada
	//-----------------------------
	uint32_t offset = 0;

	buffer->size = 2 * sizeof(uint32_t);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(tripulante->id), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(tripulante->id_patota_asociado), sizeof(uint32_t));
}

t_tarea* recibir_tarea(uint32_t socket_cliente){
	int aux;
	recv(socket_cliente, &aux, sizeof(op_code), MSG_WAITALL);
	recv(socket_cliente, &aux, sizeof(uint32_t), MSG_WAITALL);
	if(aux)
		return deserializar_solicitar_tarea_respuesta(socket_cliente);
	else
		return NULL;
}

t_list* recibir_listado_tripulantes(int socket_cliente){
	int aux;
	recv(socket_cliente, &aux, sizeof(op_code), MSG_WAITALL);
	recv(socket_cliente, &aux, sizeof(uint32_t), MSG_WAITALL);

	return deserializar_respuesta_listado_tripulantes(socket_cliente);
}

t_tarea* deserializar_solicitar_tarea_respuesta(uint32_t socket_cliente) {
	//------------ORDEN------------
	//1. Tarea
	//2. Parametro
	//3. Posicion X
	//4. Posicion Y
	//5. Tiempo
	//-----------------------------

	t_tarea* tarea = malloc(sizeof(t_tarea));
	tarea->posicion = malloc(sizeof(t_posicion));

	recv(socket_cliente, &(tarea->tamanio_tarea), sizeof(uint32_t), 0);
	tarea->tarea = malloc(tarea->tamanio_tarea);
	recv(socket_cliente, tarea->tarea, tarea->tamanio_tarea, 0);
	recv(socket_cliente, &(tarea->parametro), sizeof(uint32_t), 0);
	recv(socket_cliente, &(tarea->posicion->pos_x), sizeof(uint32_t), 0);
	recv(socket_cliente, &(tarea->posicion->pos_y), sizeof(uint32_t), 0);
	recv(socket_cliente, &(tarea->tiempo), sizeof(uint32_t), 0);

	return tarea;
}

t_posicion* deserializar_posicion_sabotaje(uint32_t socket_cliente) {
	//------------ORDEN------------
	//1. Posicion x
	//2. Posicion y
	//-----------------------------

	t_posicion* posicion = malloc(sizeof(t_posicion));
	recv(socket_cliente, &(posicion->pos_x), sizeof(uint32_t), 0);
	recv(socket_cliente, &(posicion->pos_y), sizeof(uint32_t), 0);

	return posicion;
}

t_list* deserializar_respuesta_listado_tripulantes(uint32_t socket_cliente) {
	//------------ORDEN------------
	//1. Cantidad tripulantes
	//2. Listado de tripulantes							} COMO ES LISTA IRA DENTRO DE UN FOR
		//	1.1. Tripulante
		//	1.2. Patota
		//	1.3. Estado
	//-----------------------------

	t_list* listado_tripulantes = list_create();
	uint32_t cantidad_tripulantes;

	recv(socket_cliente, &(cantidad_tripulantes), sizeof(uint32_t), 0);

	for (int i = 0; i < cantidad_tripulantes; i++) {
		t_respuesta_listar_tripulante* tripulante = malloc(sizeof(t_respuesta_listar_tripulante));

		recv(socket_cliente, &(tripulante->id_tripulante), sizeof(uint32_t), 0);
		recv(socket_cliente, &(tripulante->id_patota), sizeof(uint32_t), 0);
		recv(socket_cliente, &(tripulante->estado), sizeof(char), 0);

		list_add(listado_tripulantes, tripulante);

	}
	return listado_tripulantes;
}

