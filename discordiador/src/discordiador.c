#include "discordiador.h"

void test(){
	char* test = string_duplicate("AAAAA");
	char* test_2 = string_duplicate("AAAAA");
	char* test_3 = string_duplicate("AAA");

	char* stream_1 = string_new();
	string_append(&stream_1, test);
	string_append(&stream_1, test_2);
	string_append(&stream_1, test_3);



	printf("stream leido %s size %i\n",stream_1, string_length(stream_1));
	free(stream_1);
	void* aux = malloc(20);
	int offset = 0;
	memcpy(aux + offset,test,string_length(test));
	offset += string_length(test);
	memcpy(aux + offset,test_2,string_length(test_2));
	offset += string_length(test_2);
	memcpy(aux + offset,test_3,string_length(test_3)+1);
	offset += string_length(test_3)+1;
	free(test);
	free(test_2);
	free(test_3);
	char* aux_1 = calloc(1, 5);
	char* aux_2 = calloc(1, 5);
	char* aux_3 = calloc(1, 5);


	int offset_2 = 0;
	memcpy(aux_1,aux + offset_2,5);
	offset_2 += 5;
	memcpy(aux_2,aux + offset_2,5);
	offset_2 += 5;
	memcpy(aux_3,aux + offset_2,5);
	offset_2 += 5;

	char* stream = string_new();
	string_append(&stream, aux_1);
	string_append(&stream, aux_2);
	string_append(&stream, aux_3);

	free(aux_1);
	free(aux_2);
	free(aux_3);

	printf("stream leido %s size %i\n",stream, string_length(stream));
}

int main(int argc, char* argv[]) {
//	test();

//	p_tripulante* tripulante_plani = malloc(sizeof(p_tripulante));
//	t_tripulante* tripulante = malloc(sizeof(t_tripulante));
//	tripulante->id = 1;
//	tripulante_plani->tripulante = tripulante;
//	bool es_tripulante_plani(p_tripulante* tripulante_plani_aux){
//		if(tripulante_plani_aux && tripulante_plani)
//			if(tripulante_plani_aux->tripulante && tripulante_plani->tripulante)
//				return tripulante_plani_aux->tripulante->id == tripulante_plani->tripulante->id;
//			else
//				return false;
//		else
//			return false;
//	}
//	t_list* lista = list_create();
//	list_add(lista, tripulante_plani);
//	free(tripulante);
//	tripulante = NULL;
//	if(tripulante)
//		printf("\n\t\tENTRO TRIPU\n");
//	p_tripulante* tripulante_plani_aux = list_remove_by_condition(lista, es_tripulante_plani);
//	if(tripulante_plani_aux!=NULL && tripulante_plani_aux->tripulante!=NULL)
//		printf("\n\t\tENTRO %i\n", tripulante_plani_aux->tripulante->id);

	iniciar_discordiador();
	pthread_create(&hiloReceive, NULL, (void*) arrancar_servidor, NULL);
	pthread_detach(hiloReceive);

	leer_consola();

	finalizar_discordiador();

    return EXIT_SUCCESS;
}

void iniciar_discordiador() {
	iniciar_patotas();
	leer_config();

	logger = iniciar_logger(archivo_log, "discordiador.c");
    log_info(logger, "Ya obtuvimos la config de discordiador\n");

    conectar_modulos();
    preparar_planificacion();
    arrancar_planificacion();
}
void iniciar_patotas(){
	cantidad_patotas = 0;
	cantidad_tripulantes = 0;
//	patotas = list_create(); VER SI ES NECESARIO
}

void leer_config() {
	config = config_create(ARCHIVO_CONFIG);

	ip_discordiador = config_get_string_value(config, "IP_DISCORDIADOR");
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	ip_Mi_RAM_HQ = config_get_string_value(config, "IP_MI_RAM_HQ");
	puerto_Mi_RAM_HQ = config_get_string_value(config, "PUERTO_MI_RAM_HQ");
	ip_Mongo_Store = config_get_string_value(config, "IP_I_MONGO_STORE");
	puerto_Mongo_Store = config_get_string_value(config, "PUERTO_I_MONGO_STORE");
	archivo_log = config_get_string_value(config, "PATH_ARCHIVO_LOG");
	algoritmo = config_get_string_value(config, "ALGORITMO");
	if(son_iguales(algoritmo, "RR"))
		quantum = config_get_int_value(config, "QUANTUM");
	grado_multitarea = config_get_int_value(config, "GRADO_MULTITAREA");
	duracion_sabotaje = config_get_int_value(config, "DURACION_SABOTAJE");
	retardo_ciclo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");
}

void conectar_modulos(){
    if((socket_Mi_RAM_HQ = crear_conexion(ip_Mi_RAM_HQ, puerto_Mi_RAM_HQ)) == -1)
    	log_error(logger, "DISCORDIADOR :: No me pude conectar a Mi-RAM-HQ");
    else
		log_info(logger, "DISCORDIADOR :: Me pude conectar a Mi-RAM-HQ");
    if((socket_Mongo_Store = crear_conexion(ip_Mongo_Store, puerto_Mongo_Store)) == -1)
    	log_error(logger, "DISCORDIADOR :: No me pude conectar a i-Mongo-Store");
    else
		log_info(logger, "DISCORDIADOR :: Me pude conectar a i-Mongo-Store");

	if(socket_Mi_RAM_HQ == -1 || socket_Mongo_Store == -1)
		log_error(logger, "DISCORDIADOR :: No nos conectamos con ambos modulos\n");
	else if(socket_Mi_RAM_HQ != -1 && socket_Mongo_Store != -1)
		log_info(logger, "DISCORDIADOR :: Nos conectamos con ambos modulos\n");
}

void finalizar_discordiador(){
	log_info(logger,"\n\n\t\t\t~. Discordiador FINALIZADO .~\n");
	liberar_conexion(&socket_Mi_RAM_HQ);
	liberar_conexion(&socket_Mongo_Store);

	queue_destroy_and_destroy_elements(cola_new,free);
	queue_destroy_and_destroy_elements(cola_ready,free);
	queue_destroy_and_destroy_elements(cola_exec,free);
	queue_destroy_and_destroy_elements(cola_bloq_E_S,free);
	queue_destroy_and_destroy_elements(cola_bloq_Emergencia,free);
	queue_destroy_and_destroy_elements(cola_exit,free);

//	list_destroy_and_destroy_elements(, free);
	list_destroy(lista_bloq_Emergencia);
	list_destroy(lista_exec);
	list_destroy(lista_expulsados);
	list_destroy(lista_tripulantes_plani);
//	list_destroy_and_destroy_elements(, free);

	pthread_mutex_destroy(&mutex_cola_ready);
	pthread_mutex_destroy(&mutex_cola_exec);
	pthread_mutex_destroy(&mutex_cola_bloqueados_io);
	pthread_mutex_destroy(&mutex_planificacion_bloqueados_sabotajes);
	pthread_mutex_destroy(&mutex_sabotajes);
	pthread_mutex_destroy(&mutex_sabotajes_bloqueados_io);

	sem_destroy(&semaforo_planificacion);
	sem_destroy(&semaforo_cola_ready);
	sem_destroy(&semaforo_cola_exec);
	sem_destroy(&semaforo_cola_bloqueados_io);
	sem_destroy(&semaforo_cola_bloqueados_sabotaje);

	config_destroy(config);

	log_destroy(logger);
}
