#include "planificacion.h"

void preparar_planificacion(){
	void* convertir(char* algoritmo_nombre) {
		if (son_iguales(algoritmo_nombre, "FIFO")) return (void*) planificacion_segun_FIFO;
		if (son_iguales(algoritmo_nombre, "RR")) return (void*) planificacion_segun_RR;
		return NULL;
	}

	log_info(logger, "DISCORDIADOR :: Se crearan las colas de planificacion");
	crear_colas_planificacion();

	pthread_mutex_init(&mutex_cola_ready, NULL);
	pthread_mutex_init(&mutex_cola_exec, NULL);
	pthread_mutex_init(&mutex_cola_bloqueados_io, NULL);
	pthread_mutex_init(&mutex_planificacion_bloqueados_sabotajes, NULL);
	pthread_mutex_init(&mutex_sabotajes, NULL);
	pthread_mutex_init(&mutex_sabotajes_bloqueados_io, NULL);

	sem_init(&semaforo_planificacion, 0, 0);
	sem_init(&semaforo_cola_ready, 0, 0);
	sem_init(&semaforo_cola_exec, 0, 0);
	sem_init(&semaforo_cola_bloqueados_io, 0, 0);
	sem_init(&semaforo_cola_bloqueados_sabotaje, 0, 0);

	planificacion_corto_plazo = convertir(algoritmo);

	if(algoritmo == NULL)
		log_error(logger, "Error al leer el algoritmo de planificacion");
	else if(son_iguales(algoritmo, "RR"))
		log_info(logger, "El algoritmo seleccionado es RR con quantum de %d", quantum);
	else if(son_iguales(algoritmo, "FIFO"))
		log_info(logger, "El algoritmo seleccionado es FIFO");
	else
		log_info(logger, "El algoritmo seleccionado no coincide con ningun algoritmo valido");


}

void planificar_patota(t_patota* patota){
	log_info(logger, "DISCORDIADOR :: Crearemos los tripulantes de la patota");
	for(int i=0; i < patota->cant_tripulantes; i++){
		t_tripulante* tripulante = obtener_tripulante_de_patota(patota, i);
		log_info(logger, "DISCORDIADOR :: Se crea el tripulante %i", tripulante->id);
		pthread_t hiloTripulante;
		pthread_create(&hiloTripulante, NULL, (void*) ejecutar_tripulante, tripulante);
		pthread_detach(hiloTripulante);
	}


}

void planificar_tripulantes_bloqueados(){
	while(1){
		while (!cola_bloqueados_vacia()) {
			verificar_sabotaje_io();
			pthread_mutex_lock(&mutex_cola_bloqueados_io);
			p_tripulante* tripulante_plani = (p_tripulante*) queue_pop(cola_bloq_E_S);
			pthread_mutex_unlock(&mutex_cola_bloqueados_io);

//			pthread_mutex_lock(&tripulante_plani->mutex_solicitud);

			log_info(logger, "El tripulante %i iniciara la tarea en E/S %s por %i ciclos", tripulante_plani->tripulante->id, tripulante_plani->tripulante->tarea_act->tarea, tripulante_plani->tripulante->tarea_act->tiempo);

			rafaga_block_io(tripulante_plani->tripulante->tarea_act->tiempo);
			tripulante_plani->tripulante->tarea_act->tiempo = 0;

			pthread_mutex_unlock(&tripulante_plani->mutex_ejecucion); // SE LIBERA AVISANDO QUE TERMINO

		}
	}
}

void arrancar_planificacion(){
	for(int i=0; i<grado_multitarea; i++){
		pthread_t planificar_corto;
		pthread_create(&planificar_corto, NULL, (void*) planificacion_corto_plazo, NULL);
		pthread_detach(planificar_corto);
	}
	pthread_t planificador_bloqueados;
	pthread_create(&planificador_bloqueados, NULL, (void*) planificar_tripulantes_bloqueados, NULL);
	pthread_detach(planificador_bloqueados);

	for(int i=0; i<grado_multitarea; i++){
		sem_post(&semaforo_cola_bloqueados_sabotaje);
	}
}

void iniciar_planificacion(){
	for(int i=0; i<grado_multitarea; i++){
		sem_post(&semaforo_planificacion);
	}
}

void pausar_planificacion(){
	for(int i=0; i<grado_multitarea; i++){
		sem_wait(&semaforo_planificacion);
	}
}

void subir_tripulante_exec(p_tripulante* tripulante_plani){
	log_info(logger, "Se agrega al tripulante %i a EXEC", tripulante_plani->tripulante->id);
	tripulante_plani->tripulante->estado = EXEC;
	enviar_RAM_actualizar_estado(tripulante_plani->tripulante,tripulante_plani->tripulante->socket_conexion_RAM);
	pthread_mutex_lock(&mutex_cola_exec);
	list_add(lista_exec, tripulante_plani);
	pthread_mutex_unlock(&mutex_cola_exec);
	sem_post(&semaforo_cola_exec);
}

void planificacion_segun_FIFO() {
	log_info(logger, "Entramos a la planificacion POR FIFO");
	while(1){
		verificar_planificacion_activa();

		while (!cola_ready_vacia()) {
			pthread_mutex_lock(&mutex_cola_ready);
			p_tripulante* tripulante_plani = (p_tripulante*) queue_pop(cola_ready);
			pthread_mutex_unlock(&mutex_cola_ready);

			log_info(logger, "DISCORDIADOR :: Tomamos al tripulante %i en READY para poner en EXEC", tripulante_plani->tripulante->id);

			subir_tripulante_exec(tripulante_plani);

			while(verificar_planificacion_activa() && tripulante_plani->esta_activo){
				pthread_mutex_lock(&tripulante_plani->mutex_solicitud);
				if(tripulante_plani->esta_activo)
					pthread_mutex_unlock(&tripulante_plani->mutex_ejecucion);
			}
		}
	}
}

bool cola_bloqueados_vacia(){
	sem_wait(&semaforo_cola_bloqueados_io);
	pthread_mutex_lock(&mutex_cola_bloqueados_io);
	bool esta_vacia = queue_is_empty(cola_bloq_E_S);
	pthread_mutex_unlock(&mutex_cola_bloqueados_io);

	return esta_vacia;
}

bool cola_ready_vacia(){
	sem_wait(&semaforo_cola_ready);
	pthread_mutex_lock(&mutex_cola_ready);
	bool esta_vacia = queue_is_empty(cola_ready);
	pthread_mutex_unlock(&mutex_cola_ready);

	return esta_vacia;
}

bool verificar_planificacion_activa(){
	sem_wait(&semaforo_planificacion);
	sem_post(&semaforo_planificacion);
//	pthread_mutex_lock(&mutex_planificacion_bloqueados_sabotajes);
//	pthread_mutex_unlock(&mutex_planificacion_bloqueados_sabotajes);

	return true;
}

void planificacion_segun_RR() {
	log_info(logger, "Entramos a la planificacion POR RR");

	while(1){
		verificar_planificacion_activa();

		while (!cola_ready_vacia()) {
			pthread_mutex_lock(&mutex_cola_ready);
			p_tripulante* tripulante_plani = (p_tripulante*) queue_pop(cola_ready);
			pthread_mutex_unlock(&mutex_cola_ready);
			log_info(logger, "DISCORDIADOR :: Tomamos al tripulante %i en READY para poner en EXEC", tripulante_plani->tripulante->id);

			subir_tripulante_exec(tripulante_plani);
			for(int i=0; i<quantum && verificar_planificacion_activa() && tripulante_plani->esta_activo; i++) {
//				log_info(logger, "Se entro al for del trip %i", tripulante_plani->tripulante->id);
				pthread_mutex_lock(&tripulante_plani->mutex_solicitud);
//				log_info(logger, "LLego solicitud del trip %i", tripulante_plani->tripulante->id);
				if(tripulante_plani->esta_activo)
					pthread_mutex_unlock(&tripulante_plani->mutex_ejecucion);
			}
//			log_info(logger, "Salimos del for trip %i", tripulante_plani->tripulante->id);
			if(tripulante_plani->esta_activo){
				log_info(logger, "El tripulante %i esta activo y se subira de nuevo a ready", tripulante_plani->tripulante->id);
				subir_tripulante_ready(tripulante_plani);
			}
		}
	}
}

void finalizar_tripulante_plani(uint32_t id_tripulante) {
	list_add(lista_expulsados, id_tripulante);
}

void crear_colas_planificacion() {
	cola_new = queue_create();
	cola_ready = queue_create();
	cola_exec = queue_create();
	cola_exit = queue_create();
	cola_bloq_E_S = queue_create();
	cola_bloq_Emergencia = queue_create();
	lista_expulsados = list_create();
	lista_bloq_Emergencia = list_create();
	lista_exec = list_create();
	lista_tripulantes_plani = list_create();
}


