#include "utils_ram.h"

void arrancar_servidor(void){
	pthread_t thread;
	uint32_t socket_servidor = iniciar_servidor(logger, ip_Mi_RAM_HQ, puerto_escucha);
	while(1){
		uint32_t socket_cliente = esperar_cliente(logger,socket_servidor);

		int *socket_paraPasar = malloc(sizeof(int));
		*socket_paraPasar = socket_cliente;

		pthread_create(&thread, NULL, (void*) serve_client, socket_paraPasar);
		pthread_detach(thread);
	}
}

void serve_client(int* socket_que_nos_pasan) {
    uint32_t socket = *socket_que_nos_pasan;
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
	int err;
	t_tripulante* tripulante = malloc(sizeof(t_tripulante));
	recv(cliente_fd, &buffer_size, sizeof(uint32_t), MSG_WAITALL);

	// logguear quien se me conecto: quiza hay que agregarle a los paquetes el nombre del modulo que envió el paquete, no lo sé

	//Procesar mensajes recibidos
	switch (cod_op) {
		case ESTA_ON:
			log_info(logger, "Estamos on");
			enviar_respuesta(OK,cliente_fd);
		break;
		case INICIAR_PATOTA:
			;
			t_patota* patota = deserializar_iniciar_patota(cliente_fd);

			log_info(logger, "RAM :: Nos llego INICIAR_PATOTA de la patota %i del socket %i", patota->id_patota, cliente_fd);

			cargar_memoria_patota(patota);

			log_info(logger, "RAM :: Cargamos a memoria la patota %i y sus tareas", patota->id_patota);

			list_destroy(patota->posiciones);
			free(patota->path_tareas);
			free(patota);
		break;
		case INICIAR_TRIPULANTE:
			tripulante = deserializar_iniciar_tripulante(cliente_fd);

			log_info(logger, "RAM :: Nos llego INICIAR_TRIPULANTE del tripulante %i", tripulante->id);

			cargar_memoria_tripulante(tripulante);

			log_info(logger, "RAM :: Cargamos a memoria el tripulante %i de la patota %i", tripulante->id, tripulante->id_patota_asociado);
			err = crear_tripulante(tripulante);
			if(err) {
				log_warning(logger,"WARN: %s\n", nivel_gui_string_error(err));
			}

//			GUARDAR EN MEMORIA Y HACER LAS TARES CORRESPONDIENTES
//			POR AHORA SE HACE FREE CAPAZ DESPUES NO
//			free(tripulante);
			break;
		case LISTAR_TRIPULANTES:
			log_info(logger, "RAM :: Nos llego LISTAR_TRIPULANTES de DISCORDIADOR");
			log_info(logger, "Se procede a obtener la info de los tripulantes");


			enviar_respuesta_listado_tripulantes(obtener_listado_tripulantes(), cliente_fd);

			log_info(logger, "RAM :: Se enviaron los datos de los tripulantes");

		break;
		case EXPULSAR_TRIPULANTE:
			tripulante = deserializar_expulsar_tripulante(cliente_fd);

			log_info(logger, "RAM :: Nos llego EXPULSAR_TRIPULANTE para el tripulante %i", tripulante->id);

			// Eliminará un tripulante tanto de las estructuras administrativas de la memoria, como también del mapa
			// EN CASO QUE SEA NECESARIO eliminara su segmento de tareas

			expulsar_tripulante(tripulante);

			free(tripulante);

			log_info(logger, "RAM :: Se expulso al tripulante");

		break;
		case SOLICITAR_TAREA:
			tripulante = deserializar_solicitar_tarea(cliente_fd);

			log_info(logger, "RAM :: Nos llego SOLICITAR_TAREA para el tripulante %i", tripulante->id);

			t_tarea* tarea = obtener_tarea_memoria(tripulante);

			if(tarea == NULL){
				log_warning(logger, "RAM :: Ya no hay mas tareas a esa patota");
			}

			enviar_solicitar_tarea_respuesta(tarea,cliente_fd);

//			free(tripulante);

			log_info(logger, "RAM :: Se da la tarea");

		break;
		case MOVER_HACIA:
			;
			t_mover_hacia* mover_hacia =  deserializar_mover_hacia(cliente_fd);

			log_info(logger, "RAM :: Nos llego MOVER_HACIA del tripulante %i", mover_hacia->id_tripulante);

			mover_tripulante_memoria(mover_hacia);

			err = mover_tripulante(mover_hacia);
			if(err) {
				log_warning(logger,"WARN: %s\n", nivel_gui_string_error(err));
			}

			free(mover_hacia);

		break;
		case ACTUALIZAR_ESTADO_TRIPULANTE:
			tripulante = deserializar_actualizar_estado(cliente_fd);

			log_info(logger, "RAM :: Nos llego ACTUALIZAR_ESTADO_TRIPULANTE del tripulante %i",tripulante->id);

			modificar_memoria_estado_tripulante(tripulante,estado_char(tripulante->estado));

			log_info(logger, "RAM :: Se actualizo el estado del tripulante.");

			free(tripulante);
		break;
	}
}

char estado_char(t_estado estado){
    switch(estado){
        case NEW:
            return 'N';
        case READY:
            return 'R';
        case EXEC:
            return 'E';
        case BLOCKED_I_O: case BLOCKED_EMERG:
            return 'B';
    }
    return 'X';
}

/*
void eliminar_patota_de_swap(uint32_t idPatota){
	bool entradas_de_la_patota(void* parametro) {
		entradaSwap* entrada = (entradaSwap *) parametro;
		return idPatota == entrada->idPatota;
	}
	t_list* marcosEnSwap = list_filter(entradas_swap, entradas_de_la_patota);

	for(int i=0; i < list_size(marcosEnSwap); i++){
		entradaSwap* entrada = list_get(marcosEnSwap,i);
		entrada->libre = true;

		borrar_de_swap(entrada->indiceMarcoSwap);
	}

}
*/


t_tarea* obtener_tarea(t_tripulante* tripulante){
	char* tareas = (char*) leer_memoria(tripulante->id, tripulante->id_patota_asociado, TAREAS);
	log_info(logger, "RAM :: Tareas obtenida:\n%s", tareas);

	char** lines = string_split(tareas, "\n");
	log_info(logger, "RAM :: Tarea obtenida:\n%s", lines[0]);
	t_tarea* tarea = obtener_tarea_archivo(lines[0]);

	free(lines);

	return tarea;
}


char* obtener_tareas(t_patota* patota){
	char* leido = string_new();
	char* caracter = calloc(2,sizeof(char));

	FILE* archivo = fopen(patota->path_tareas, "r+");

	if(archivo){
		log_info(logger, "Se pudo abrir el archivo:\n%s",patota->path_tareas);
//		log_info(logger, "El archivo:\n%s\nY el primer caracter es:\n%s",patota->path_tareas, fgetc(archivo));
		while(fread(caracter,1,1,archivo)){
			string_append(&leido, caracter);
		}
		free(caracter);
		log_info(logger, "El archivo:\n%s\nTiene las tareas:\n%s",patota->path_tareas, leido);

		return leido;
	}
	log_error(logger, "No se pudo abrir el archivo:\n%s",patota->path_tareas);

	return NULL;
}

t_tarea* obtener_tarea_archivo(char* tarea_string){
	log_info(logger, "TAREA: %s", tarea_string);
	t_tarea* tarea = malloc(sizeof(t_tarea));
	tarea->posicion = malloc(sizeof(t_posicion));
	char** tarea_parametros;
	char** parametros;

	if(string_contains(tarea_string," ")){
		tarea_parametros = string_n_split(tarea_string, 2, " ");
		parametros = string_n_split(tarea_parametros[1], 4, ";");
		tarea->parametro = atoi(parametros[0]);
		tarea->posicion->pos_x = atoi(parametros[1]);
		tarea->posicion->pos_y = atoi(parametros[2]);
		tarea->tiempo = atoi(parametros[3]);
	}
	else{
		tarea_parametros = string_n_split(tarea_string, 4, ";");
		tarea->parametro = 0;
		tarea->posicion->pos_x = atoi(tarea_parametros[1]);
		tarea->posicion->pos_y = atoi(tarea_parametros[2]);
		tarea->tiempo = atoi(tarea_parametros[3]);
	}

	tarea->tarea = string_duplicate(tarea_parametros[0]);

	return tarea;
}

t_respuesta_listar_tripulante* de_tcb_a_listar(t_tcb* tcb, uint32_t id_patota){
	t_respuesta_listar_tripulante* listar = malloc(sizeof(t_respuesta_listar_tripulante));

	listar->estado = tcb->estado;
	listar->id_tripulante = tcb->tid;
	listar->id_patota = id_patota;

	return listar;
}

t_list* obtener_listado_tripulantes() {
	t_list* listado_de_tripulantes = list_create();
	pthread_mutex_lock(&mutex_tablas);
	if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		for(int i=0; i<list_size(lista_tablas_segmentos); i++){
			t_tabla_segmentos* tabla = (t_tabla_segmentos*) list_get(lista_tablas_segmentos, i);

			for(int j=0; j<list_size(tabla->tripulantes_activos); j++){
				t_tcb* tcb = (t_tcb*) obtener_tripulante_memoria(list_get(tabla->tripulantes_activos,j),tabla->id_patota_asociada);
				list_add(listado_de_tripulantes, de_tcb_a_listar(tcb,tabla->id_patota_asociada));
			}
		}

	}
	else if(son_iguales(esquema_memoria, "PAGINACION")) {
		for(int i=0; i<list_size(lista_tablas_paginas); i++){
			t_tabla_paginas* tabla = (t_tabla_paginas*) list_get(lista_tablas_paginas, i);

			for(int j=0; j<list_size(tabla->tripulantes_activos); j++){
				t_tcb* tcb = (t_tcb*) obtener_tripulante_memoria(list_get(tabla->tripulantes_activos,j),tabla->id_patota_asociada);
				list_add(listado_de_tripulantes, de_tcb_a_listar(tcb,tabla->id_patota_asociada));
			}
		}

	}
	pthread_mutex_unlock(&mutex_tablas);
	return listado_de_tripulantes;
}


