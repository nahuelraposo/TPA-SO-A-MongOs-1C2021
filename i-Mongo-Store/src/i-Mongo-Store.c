#include "i-Mongo-Store.h"


int main(int argc, char* argv[]) {
	iniciar_mongo();
	pthread_create(&hiloReceive, NULL, (void*) arrancar_servidor, NULL);
	pthread_join(hiloReceive, NULL);
    return EXIT_SUCCESS;
}

void iniciar_mongo(void) {
	leer_config();
	logger = iniciar_logger(archivo_log, "i-Mongo-Store.c");
    log_info(logger, "Ya obtuvimos la config del mongo");
    FS_RESET();

    inicializar_filesystem();
    verificar_sabotajes();
	signal(SIGINT, finalizar_i_mongo);

//    if((socket_discordiador = crear_conexion(ip_discordiador, puerto_discordiador)) == -1)
//    	log_error(logger, "MONGO STORE :: No me pude conectar al DISCORDIADOR");
//    else
//    	log_info(logger, "MONGO STORE:: Pude conectar al DISCORDIADOR");

}

void inicializar_filesystem() {

    inicializar_paths_aux();
	if(!archivo_existe(path_superbloque))
		crear_superbloque();
	else
	    obtener_superbloque();

	if(!archivo_existe(path_blocks))
	    crear_blocks();
	else
		obtener_blocks();

	pthread_mutex_init(&mutex_FS, NULL);

	pthread_t hilo_sincronizador;
	pthread_create(&hilo_sincronizador, NULL, (void*) sincronizar_blocks, NULL);
	pthread_detach(hilo_sincronizador);
}

void FS_RESET(){
//	t_list* bloques_bitacoras = obtener_bloques_bitacora();
//	for(int i=0; i<list_size(bloques_bitacoras);i++)
//		liberar_bloque((int) list_get(bloques_bitacoras,i));
//	char* comando = string_new();
//	string_append_with_format(&comando, "rm -R %s/Files/Bitacoras/ --dir", punto_montaje);
//	system(comando);
//	free(comando);
//	list_destroy(bloques_bitacoras);

	uint32_t borrarFS;
	printf("Desea reiniciar el FileSystem? (SI=1 | NO=0) :");
	scanf("%d",&borrarFS);

	if(borrarFS){
		char* aux = string_new();
		string_append_with_format(&aux, "sh ../volar_polus.sh %s", punto_montaje);
		system(aux);
		free(aux);
	}
}

void leer_config(void) {
	config = config_create(ARCHIVO_CONFIG);

	punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE");
	archivo_log = config_get_string_value(config, "PATH_ARCHIVO_LOG");
	tiempo_sincronizacion = config_get_int_value(config, "TIEMPO_SINCRONIZACION");

	ip_Mongo_Store = config_get_string_value(config, "IP_I_MONGO_STORE");
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");

	ip_discordiador = config_get_string_value(config, "IP_DISCORDIADOR");
	puerto_discordiador = config_get_string_value(config, "PUERTO_DISCORDIADOR");

	blocks = config_get_int_value(config, "BLOCKS");
	block_size = config_get_int_value(config, "BLOCK_SIZE");

	posiciones_sabotaje = list_create();
	obtener_posiciones_sabotaje();

	proxima_posicion_sabotaje = 0;
}

void obtener_posiciones_sabotaje() {
	char** posiciones_sabotaje_strings = config_get_array_value(config, "POSICIONES_SABOTAJE");
	for(uint32_t i = 0; posiciones_sabotaje_strings[i] != NULL; i++) {
		t_posicion* posicion = malloc(sizeof(t_posicion));
		char** posicion_string = string_split(posiciones_sabotaje_strings[i], "|");

		posicion->pos_x = (uint32_t) atoi(posicion_string[0]);
		posicion->pos_y = (uint32_t) atoi(posicion_string[1]);

		list_add(posiciones_sabotaje, posicion);
		string_iterate_lines(posicion_string, (void*) free);
	}
}

void finalizar_i_mongo(int signum) {
//	liberar_conexion(&socket_Mi_RAM_HQ);
	log_info(logger,"\n\t\t\t~. MONGO STORE FINALIZADO .~\n");
	log_destroy(logger);

	config_destroy(config);

	//pthread_mutex_destroy(mutexBlocks);
	free(path_blocks);
	//free(bitarrayFS);
	free(path_superbloque);
//	free(mutexBlocks);
	free(path_files);
	free(path_bitacoras);
	//TODO Terminar conexiones...
	exit(0);
}
