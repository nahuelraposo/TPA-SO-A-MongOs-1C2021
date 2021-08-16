#include "msg_Mongo.h"

void enviar_discordiador_sabotaje(t_posicion* posicion_sabotaje, uint32_t socket_discordiador) {
	t_paquete* paquete_a_enviar = crear_paquete(SABOTAJE);
	serializar_sabotaje_en_mongo(posicion_sabotaje, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_discordiador);
}

void enviar_respuesta_obtener_bitacora(char* bitacora, uint32_t socket_discordiador) {
	t_paquete* paquete_a_enviar = crear_paquete(OBTENER_BITACORA_RESPUESTA);
	serializar_obtener_bitacora(bitacora, paquete_a_enviar->buffer);
	enviar_paquete(paquete_a_enviar, socket_discordiador);
}

void serializar_obtener_bitacora(char* bitacora, t_buffer* buffer) {
	//------------ORDEN------------
	//1. Tam bitacora
	//2. Bitacora
	//-----------------------------
	uint32_t offset = 0;
	uint32_t tamanio_bitacora = string_length(bitacora)+1;
	buffer->size = tamanio_bitacora + sizeof(uint32_t);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(tamanio_bitacora), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, bitacora, tamanio_bitacora);
}


t_mover_hacia* deserializar_mover_hacia_posicion(uint32_t socket_cliente){
	//------------ORDEN------------
	//1. ID
	//2. Posicion origen
	//3. Posicion destino
	//-----------------------------

	t_mover_hacia* posicion = malloc(sizeof(t_mover_hacia));
	posicion->posicion_origen = malloc(sizeof(t_posicion));
	posicion->posicion_destino = malloc(sizeof(t_posicion));

	recv(socket_cliente, &(posicion->id_tripulante), sizeof(uint32_t), 0);
	recv(socket_cliente, &(posicion->posicion_origen->pos_x), sizeof(uint32_t), 0);
	recv(socket_cliente, &(posicion->posicion_origen->pos_y), sizeof(uint32_t), 0);
	recv(socket_cliente, &(posicion->posicion_destino->pos_x), sizeof(uint32_t), 0);
	recv(socket_cliente, &(posicion->posicion_destino->pos_y), sizeof(uint32_t), 0);

	return posicion;
}

t_tarea_Mongo* deserializar_tarea(uint32_t socket_cliente) {
	//------------ORDEN------------
	//1. ID
	//2. Tamanio
	//3. Tarea
	//4. Parametro
	//-----------------------------

	t_tarea_Mongo* tarea = malloc(sizeof(t_tarea_Mongo));

	recv(socket_cliente, &(tarea->id), sizeof(uint32_t), 0);
	recv(socket_cliente, &(tarea->tamanio_tarea), sizeof(uint32_t), 0);
	tarea->tarea = calloc(1,tarea->tamanio_tarea);
	recv(socket_cliente, tarea->tarea, tarea->tamanio_tarea, 0);
	recv(socket_cliente, &(tarea->parametro), sizeof(uint32_t), 0);

	return tarea;
}

t_bitacora_tarea_Mongo* deserializar_bitacora_tarea(uint32_t socket_cliente) {
	//------------ORDEN------------
	//1. ID
	//2. Tamanio
	//3. Tarea
	//-----------------------------

	t_bitacora_tarea_Mongo* tarea = malloc(sizeof(t_bitacora_tarea_Mongo));

	recv(socket_cliente, &(tarea->id), sizeof(uint32_t), 0);
	recv(socket_cliente, &(tarea->tamanio_tarea), sizeof(uint32_t), 0);
	tarea->tarea = calloc(1,tarea->tamanio_tarea);
	recv(socket_cliente, tarea->tarea, tarea->tamanio_tarea, 0);

	return tarea;
}

void serializar_sabotaje_en_mongo(t_posicion* posicion, t_buffer* buffer) {
	//------------ORDEN------------
	//1. Posicion x
	//2. Posicion y
	//-----------------------------
	uint32_t offset = 0;
	buffer->size = sizeof(t_posicion);
	buffer->stream = malloc(buffer->size);

	memcpy(buffer->stream + offset, &(posicion->pos_x), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(buffer->stream + offset, &(posicion->pos_y), sizeof(uint32_t));
	offset += sizeof(uint32_t);
}

t_tripulante* deserializar_tripulante_sabotaje(int socket_cliente){
	//------------ORDEN------------
	//1. ID
	//2. Patota asociada
	//-----------------------------
	t_tripulante* msg = malloc(sizeof(t_tripulante));

	recv(socket_cliente, &(msg->id), sizeof(uint32_t), 0);
	recv(socket_cliente, &(msg->id_patota_asociado), sizeof(uint32_t), 0);

	return msg;
}

t_tripulante* recibir_tripulante_sabotaje(int socket_cliente){
	int aux;
	recv(socket_cliente, &aux, sizeof(op_code), MSG_WAITALL);
	recv(socket_cliente, &aux, sizeof(uint32_t), MSG_WAITALL);

	return deserializar_tripulante_sabotaje(socket_cliente);
}



