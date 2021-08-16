#include "Mi-RAM-HQ.h"

int main(int argc, char* argv[]) {
	iniciar_Mi_RAM_HQ();

	pthread_create(&hiloReceive, NULL, (void*) arrancar_servidor, NULL);
	pthread_join(hiloReceive, NULL);

    return EXIT_SUCCESS;
}

void iniciar_Mi_RAM_HQ() {
	leer_config();
	logger = iniciar_logger_RAM(archivo_log, "Mi-RAM-HQ.c");
    log_info(logger, "Ya obtuvimos la config de Mi RAM HQ\n");
//    log_info(logger, "El pid de Mi-RAM-HQ es %i\n",process_getpid());

    iniciar_memoria();

	signal(SIGINT, finalizar_mi_ram);
}

t_log* iniciar_logger_RAM(char* archivo_log, char* archivo_c) {
	return log_create(archivo_log, archivo_c, 0, LOG_LEVEL_INFO);
}

void leer_config() {
	config = config_create(ARCHIVO_CONFIG);

	archivo_log = config_get_string_value(config, "PATH_ARCHIVO_LOG");
	ip_Mi_RAM_HQ = config_get_string_value(config, "IP_RAM");
	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");

	tamanio_memoria = config_get_int_value(config, "TAMANIO_MEMORIA");
	esquema_memoria = config_get_string_value(config, "ESQUEMA_MEMORIA");

	ip_discordiador = config_get_string_value(config, "IP_DISCORDIADOR");
	puerto_discordiador = config_get_string_value(config, "PUERTO_DISCORDIADOR");

	if(son_iguales(esquema_memoria,"PAGINACION")){
		tamanio_pagina = config_get_int_value(config, "TAMANIO_PAGINA");
		tamanio_swap = config_get_int_value(config, "TAMANIO_SWAP");
		path_swap = config_get_string_value(config, "PATH_SWAP");
		algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	}
	else{
		criterio_seleccion = config_get_string_value(config, "CRITERIO_SELECCION");
	}
}

void finalizar_mi_ram(int signum){
	log_info(logger,"\n\t\t\t~. Mi-RAM-HQ FINALIZADO .~\n");

	if(son_iguales(esquema_memoria, "SEGMENTACION")) {
		list_destroy(lista_segmentos_libres);
		list_destroy(lista_tablas_segmentos);
		list_destroy(tabla_asociadores_segmentos);
	}
	else if(son_iguales(esquema_memoria, "PAGINACION")) {
		free(path_swap);
//		free(seleccionar_victima);
//		free(algoritmo_reemplazo);
//		free(criterio_seleccion);
//		free(memoria_virtual);

		list_destroy(lista_tablas_paginas);
		list_destroy(tablaDeMarcos);
		list_destroy(marcos_swap);
	}

	free(memoria);
//	free(esquema_memoria);
//	free(ip_discordiador);
//	free(puerto_discordiador);
//	free(ip_Mi_RAM_HQ);
//	free(puerto_escucha);
	log_destroy(logger);
//	config_destroy(config);

	exit(0);
}
