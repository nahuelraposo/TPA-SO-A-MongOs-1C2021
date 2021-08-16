#include "utils_discordiador.h"

void arrancar_servidor(void){
	pthread_t thread;
	uint32_t socket_servidor = iniciar_servidor(logger, ip_discordiador, puerto_escucha);
	while(1){
		uint32_t socket_cliente = esperar_cliente(logger,socket_servidor);

		int *socket_para_pasar = malloc(sizeof(int));
		*socket_para_pasar = socket_cliente;

		pthread_create(&thread, NULL, (void*) serve_client, socket_para_pasar);
		pthread_detach(thread);
	}
}

void serve_client(int* socket_que_nos_pasan) {
    int socket = *socket_que_nos_pasan;
	free(socket_que_nos_pasan);

	int cod_op;

	while (1) {
		int i = recv(socket, &cod_op, sizeof(op_code), MSG_WAITALL);

		if (i != 4) {
			cod_op = -1;
			liberar_conexion(&socket);
			pthread_exit(NULL);
		}
		else {
			procesar_mensaje_recibido(cod_op, socket);
		}
	}

}

void procesar_mensaje_recibido(int cod_op, int cliente_fd) {

	uint32_t buffer_size;
	recv(cliente_fd, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	// logguear quien se me conecto: quiza hay que agregarle a los paquetes el nombre del modulo que envió el paquete, no lo sé

	//Procesar mensajes recibidos
	switch (cod_op) {
		case ESTA_ON:
			log_info(logger, "Estamos on");
			enviar_respuesta(OK,cliente_fd);
		break;

//		case RESPUESTA_LISTAR_TRIPULANTES:
//			log_info(logger, "NOS LLEGO EL LISTADO DE LOS TRIPULANTES");
//			listado_recibido = (t_respuesta_listado_tripulantes*) deserializar_respuesta_listado_tripulantes(cliente_fd);
//
//			if (listado_recibido->cantidad_tripulantes == 0)
//				log_warning(logger, "DISCORDIADOR :: No pudimos obtener informacion de ninguno de los tripulantes de Mi-RAM");
//			else {
//				t_respuesta_listar_tripulante* respuesta = malloc(sizeof(t_respuesta_listar_tripulante));
//
//				log_info(logger, "--------------------------------------------------------");
//				log_info(logger, "DISCORDIADOR :: Estado de la nave: %s", temporal_get_string_time("%d/%m/%y %H:%M:%S"));
//				log_info(logger, "--------------------------------------------------------");
//
//				for(int i = 0; i < listado_recibido->cantidad_tripulantes; i++) {
//					respuesta = (t_respuesta_listar_tripulante*) list_get(listado_recibido->tripulantes, i);
//					log_info(
//							logger,
//							"Tripulante: %i\t Patota: %i\t Estado: %s",
//							respuesta->id_tripulante,
//							respuesta->id_patota,
//							obtener_estado_segun_caracter(respuesta->estado)
//						);
//					free(respuesta);
//					// POR AHORA SE PRUEBA CON TRIPULANTES HARDCODEADOS EN RAM NADA MAS
//				}
//				free(respuesta);
//			}
//			log_info(logger, "--------------------------------------------------------");
//		break;

		case SABOTAJE:
			posicion_sabotaje = deserializar_posicion_sabotaje(cliente_fd);
			log_info(logger, "DISCORDIADOR :: Me llego SABOTAJE de MONGO STORE en la posicion %i|%i",
					posicion_sabotaje->pos_x,
					posicion_sabotaje->pos_y
					);
			log_info(logger, "El sabotaje tendra una duracion de %d", duracion_sabotaje);

			detener_tripulantes();
			planificar_tripulante_para_sabotaje(cliente_fd);
			desbloquear_por_sabotaje();
			free(posicion_sabotaje);
			break;
	}

}

void liberar_tripulante_plani(p_tripulante* tripulante_plani){
	if(tripulante_plani == NULL)
		return;
	pthread_mutex_destroy(&tripulante_plani->mutex_ejecucion);
	pthread_mutex_destroy(&tripulante_plani->mutex_solicitud);
//	free(tripulante_plani);
//	tripulante_plani = NULL;
}

p_tripulante* quitar_tripulante_plani(uint32_t id_tripulante){
	bool es_id(p_tripulante* tripulante_plani){
		return id_tripulante == tripulante_plani->tripulante->id;
	}
	bool es_id_tripulante(t_tripulante* tripulante){
		return id_tripulante == tripulante->id;
	}

	p_tripulante* tripulante_plani = list_remove_by_condition(lista_tripulantes_plani,es_id);
	if(tripulante_plani != NULL){
		switch(tripulante_plani->tripulante->estado){
			case READY:
				for(int i=0; i<queue_size(cola_ready); i++){
					pthread_mutex_lock(&mutex_cola_ready);
					p_tripulante* tripulante_plani_aux = (p_tripulante*) queue_pop(cola_ready);
					if(tripulante_plani_aux->tripulante->id != id_tripulante)
						queue_push(cola_ready, tripulante_plani_aux); // se devuelven a la cola
					pthread_mutex_unlock(&mutex_cola_ready);
				}
				break;
			case EXEC:
				list_remove_by_condition(lista_exec,es_id_tripulante);
		}
	}

	return tripulante_plani;
}

void detener_tripulantes(){
	bloquear_por_sabotaje();
	for(int i=0; i<list_size(lista_exec); i++){
		pthread_mutex_lock(&mutex_cola_exec);
		p_tripulante* tripulante_plani = (p_tripulante*) list_get(lista_exec,i);
		pthread_mutex_unlock(&mutex_cola_exec);

//		log_info(logger, "Detenemos al tripulante %d en EXEC", tripulante_plani->tripulante->id);
//		pthread_mutex_lock(&tripulante_plani->mutex_ejecucion);

		list_add(lista_bloq_Emergencia, tripulante_plani->tripulante);
	}
	for(int i=0; i<queue_size(cola_ready); i++){
		pthread_mutex_lock(&mutex_cola_ready);
		p_tripulante* tripulante_plani = (p_tripulante*) queue_pop(cola_ready);
		queue_push(cola_ready, tripulante_plani); // se devuelven a la cola
		pthread_mutex_unlock(&mutex_cola_ready);

//		log_info(logger, "Detenemos al tripulante %d en READY", tripulante_plani->tripulante->id);
		list_add(lista_bloq_Emergencia, tripulante_plani->tripulante);
	}
//	for(int i=0; i<queue_size(cola_bloq_E_S); i++){
//		pthread_mutex_lock(&mutex_cola_bloqueados_io);
//		p_tripulante* tripulante_plani = (p_tripulante*) queue_pop(cola_bloq_E_S);
//		queue_push(cola_bloq_E_S, tripulante_plani); // se devuelven a la cola
//		pthread_mutex_unlock(&mutex_cola_bloqueados_io);
//
//		log_info(logger, "Detenemos al tripulante %d en BLOQ I/O", tripulante_plani->tripulante->id);
//		list_add(lista_bloq_Emergencia, tripulante_plani->tripulante);
//	}

	log_info(logger,
			"Detuvimos a los tripulantes durante el sabotaje en la posicion %d|%d",
			posicion_sabotaje->pos_x,
			posicion_sabotaje->pos_y
			);
}

void bloquear_por_sabotaje(){
	pthread_mutex_lock(&mutex_sabotajes);
	pthread_mutex_lock(&mutex_sabotajes_bloqueados_io);
	for(int i=0; i<grado_multitarea; i++){
		sem_wait(&semaforo_cola_bloqueados_sabotaje);
	}
}

void desbloquear_por_sabotaje(){
	pthread_mutex_unlock(&mutex_sabotajes);
	pthread_mutex_unlock(&mutex_sabotajes_bloqueados_io);
	for(int i=0; i<grado_multitarea; i++){
		sem_post(&semaforo_cola_bloqueados_sabotaje);
	}
}


bool verificar_sabotaje_cpu(){
	sem_wait(&semaforo_cola_bloqueados_sabotaje);
	sem_post(&semaforo_cola_bloqueados_sabotaje);

	return true;
}

bool verificar_sabotaje_io(){
	pthread_mutex_lock(&mutex_sabotajes_bloqueados_io);
	pthread_mutex_unlock(&mutex_sabotajes_bloqueados_io);

	return true;
}

void rafaga_cpu(uint32_t tiempo){
	for(int i=0; i<tiempo && verificar_sabotaje_cpu(); i++)
		rafaga(1);
}

void rafaga(int tiempo){
	sleep(retardo_ciclo_cpu*tiempo);
}

void rafaga_block_io(uint32_t tiempo){
	for(int i=0; i<tiempo && verificar_sabotaje_io(); i++)
		rafaga(1);
}

char* obtener_estado_segun_caracter(char estado) {
	char* estado_string;
	switch(estado) {
		case 'N':
			estado_string = "NEW";
			break;
		case 'R':
			estado_string =  "READY";
			break;
		case 'B':
			estado_string =  "BLOCK I/O";
			break;
		case 'E':
			estado_string =  "EXEC";
			break;
		default:
			estado_string = "ERROR";
			break;
	}
	return estado_string;
}

int conectar_con_RAM(){
	int socket_con_RAM;
	if((socket_con_RAM = crear_conexion(ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ)) == -1)
		log_error(logger, "DISCORDIADOR :: No me pude conectar a Mi-RAM-HQ");
	else
		log_info(logger, "DISCORDIADOR :: Me pude conectar a Mi-RAM-HQ");

	return socket_con_RAM;
}

int conectar_con_MONGO(){
	int socket_con_MONGO;
	if((socket_con_MONGO = crear_conexion(ip_Mongo_Store, puerto_Mongo_Store)) == -1)
		log_error(logger, "DISCORDIADOR :: No me pude conectar a i-Mongo-Store");
	else
		log_info(logger, "DISCORDIADOR :: Me pude conectar a i-Mongo-Store");

	return socket_con_MONGO;
}

// ELIJO A LA POSICION_A -> true
// ELIJO A LA POSICION_B -> false
bool posicion_mas_cercana(t_posicion* posicion_A, t_posicion* posicion_B, t_posicion* posicion_destino){
	bool resultado = true;

//	int primer_termino_A = pow(posicion_destino->pos_x - posicion_A->pos_x, 2);
//	int segundo_termino_A = pow(posicion_destino->pos_y - posicion_A->pos_y, 2);
//
//	int primer_termino_B = pow(posicion_destino->pos_x - posicion_B->pos_x, 2);
//	int segundo_termino_B = pow(posicion_destino->pos_y - posicion_B->pos_y, 2);
//
//	int distancia_A = sqrt(primer_termino_A + segundo_termino_A);
//	int distancia_B = sqrt(primer_termino_B + segundo_termino_B);

	int distancia_pitagoras_A = pitagoras_entre(posicion_A, posicion_destino);
	int distancia_pitagoras_B = pitagoras_entre(posicion_B, posicion_destino);

	if(distancia_pitagoras_A < distancia_pitagoras_B)
		resultado = true;
	else if(distancia_pitagoras_A > distancia_pitagoras_B)
		resultado = false;

	return resultado;
}

int pitagoras_entre(t_posicion* posicion_A, t_posicion* posicion_B){

	int diferencia_al_cuadrado(int punto_A, int punto_B){
		return pow(punto_A - punto_B, 2);
	}

	int primer_termino = diferencia_al_cuadrado(posicion_A->pos_x, posicion_B->pos_x);
	int segundo_termino = diferencia_al_cuadrado(posicion_A->pos_y, posicion_B->pos_y);

	return sqrt(primer_termino + segundo_termino);
}

char* obtener_estado_segun_enum(int estado) {
	char* estado_string;
	switch(estado) {
		case NEW:
			estado_string = "NEW";
			break;
		case READY:
			estado_string = "READY";
			break;
		case BLOCKED_I_O: case BLOCKED_EMERG:
			estado_string =  "BLOCK I/O";
			break;
		case EXEC:
			estado_string =  "EXEC";
			break;
		default:
			estado_string = "ERROR";
			break;
	}
	return estado_string;
}

void planificar_tripulante_para_sabotaje(int socket_mongo){

	bool tripulante_en_exec_o_ready(t_tripulante* tripulante) {
		char* estado = obtener_estado_segun_enum(tripulante->estado);
		return estado == "EXEC" || estado == "READY";
	}

	bool ordenar_segun_posicion(t_tripulante* tripulante_A, t_tripulante* tripulante_B) {
		return posicion_mas_cercana(tripulante_A->posicion, tripulante_B->posicion, posicion_sabotaje);
	}

	bool ordenar_segun_id(t_tripulante* tripulante_A, t_tripulante* tripulante_B) {
		return tripulante_A->id < tripulante_B->id;
	}

	t_list* tripulantes = list_filter(lista_bloq_Emergencia, tripulante_en_exec_o_ready);
	list_clean(lista_bloq_Emergencia);

	if(list_size(tripulantes) == 0 || list_size(lista_expulsados) == cantidad_tripulantes){
		log_warning(logger, "No hay tripulantes para mandar a resolver el sabotaje. Cantidad tripulantes: %d", list_size(tripulantes));
		return;
	}

	list_sort(tripulantes, ordenar_segun_id);
	list_sort(tripulantes, ordenar_segun_posicion);
//	for(int i=0; i< list_size(tripulantes); i++){
//		t_tripulante* tripulante = list_get(tripulantes, i);
//		log_info(logger, "tripulantes[%i]: ID:%i	DISTANCIA:%d", i, tripulante->id, pitagoras_entre(tripulante->posicion, posicion_sabotaje));
//	}

	t_tripulante* tripulante_mas_cercano = list_get(tripulantes, 0);

	log_info(logger, "El tripulante %d corre en panico a la posicion del sabotaje", tripulante_mas_cercano->id);
	ejecutar_tripulante_para_sabotaje(tripulante_mas_cercano);
	log_info(logger, "El tripulante %d llego a la posicion del sabotaje", tripulante_mas_cercano->id);


	log_info(logger, "Invocando al FSCK para comenzar las correcciones correspondientes");
	enviar_Mongo_tripulante_sabotaje(tripulante_mas_cercano, socket_mongo);

	rafaga(duracion_sabotaje);
	log_info(logger, "El tripulante %d finalizo su I/O en emergencia", tripulante_mas_cercano->id);
	t_respuesta respuesta = recibir_respuesta(socket_mongo);
	if(respuesta == OK)
		log_info(logger, "Se resolvio el sabotaje");
	else
		log_warning(logger, "No hubo sabotaje");
	log_info(logger, "Volviendo a toda la tripulacion de la nave de amongo a la normalidad");

	regresar_tripulante(tripulante_mas_cercano);

}


void regresar_tripulante(t_tripulante* tripulante){
	// Muevo al tripulante a la posicion de su tarea
	while(!esta_en_el_lugar(tripulante->posicion, tripulante->tarea_act->posicion)){
		enviar_mover_hacia(tripulante,avanzar_hacia(tripulante, tripulante->tarea_act->posicion, true));
	}
}
