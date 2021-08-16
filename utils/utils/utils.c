#include "utils.h"

t_log* iniciar_logger(char* archivo_log, char* archivo_c) {
	return log_create(archivo_log, archivo_c, 1, LOG_LEVEL_INFO);
}

int contar_elementos_array(char** array) {
	int contador = 0;
	while (array[contador] != NULL) {
		contador++;
	}
	return contador;
}

uint32_t minimo(uint32_t numero1, uint32_t numero2){
	if(numero1 < numero2)
		return numero1;
	return numero2;
}

int son_iguales(char* cadenaA, char* cadenaB) {
	return strcmp(cadenaA ,cadenaB) == 0;
}

int chequear_argumentos_del_mensaje(char** argumentos, int cantidad_argumentos_mensaje) {
	return contar_elementos_array(argumentos) != cantidad_argumentos_mensaje; // ???
}

int existe_en_array(char** array, char* elemento_buscado) {
	int contador = 0;
	while (array[contador] != NULL || !son_iguales(elemento_buscado, array[contador])) {
		contador++;
	}
	if(array[contador] != NULL)
		return 1;
	else
		return 0;
}

t_posicion obtener_posicion_origen(t_posicion* posicion, t_movimiento direccion){
	t_posicion posicion_anterior = *posicion;

	switch(direccion){
		case ARRIBA:
			posicion_anterior.pos_y++;
		break;
		case ABAJO:
			posicion_anterior.pos_y--;
		break;
		case DERECHA:
			posicion_anterior.pos_x--;
		break;
		case IZQUIERDA:
			posicion_anterior.pos_x++;
		break;
	}

	return posicion_anterior;
}

// El buen helloWorld, nada le gana
void helloWorld() {
    puts("Hello world with shared!\n!");
}
