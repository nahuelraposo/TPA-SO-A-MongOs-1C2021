#include "mapa.h"

void ASSERT_CREATE(NIVEL* nivel,char id, int err) {
    if(err) {
        nivel_destruir(nivel);
        nivel_gui_terminar();
        fprintf(stderr, "Error al crear '%c': %s\n", id, nivel_gui_string_error(err));
        log_error(logger, "Error al crear '%c': %s\n", id, nivel_gui_string_error(err));
    }
}

int rnd() {
	return (rand() % 3) - 1;
}

bool mapa_activo(){
	pthread_mutex_lock(&mutex_mapa);
	return true;
}

void iniciar_mapa_vacio() {
	int cols, rows;
//	int err;

	nivel_gui_inicializar();

	nivel_gui_get_area_nivel(&cols, &rows);

	nivel = nivel_crear("La nave de amongo");

	while (mapa_activo()) {

		// Todo
		// Queda loopeando de forma media raris el mapa
		nivel_gui_dibujar(nivel);

	}

}

char asignar_letra(uint32_t id_tripulante){
	if(id_tripulante <= 26)
		return (id_tripulante + 'A' - 1);
	return (id_tripulante + 'a' - 1);
}

int crear_tripulante(t_tripulante* tripulante){
	char identificador = asignar_letra(tripulante->id);
	log_info(logger, "MAPA :: Cargamos al mapa el tripulante %i de la patota %i y tendra el identificador \"%c\"", tripulante->id, tripulante->id_patota_asociado, identificador);

	int err = personaje_crear(nivel, identificador, tripulante->posicion->pos_x, tripulante->posicion->pos_y);

	ASSERT_CREATE(nivel, identificador, err);

	pthread_mutex_unlock(&mutex_mapa);

	return err;
}

int mover_tripulante(t_mover_hacia* mover_hacia){
	int err;
	char identificador = asignar_letra(mover_hacia->id_tripulante);
	switch(mover_hacia->direccion){
		case ARRIBA:
			err = item_desplazar(nivel, identificador, 0, -1);
		break;
		case ABAJO:
			err = item_desplazar(nivel, identificador, 0, 1);
		break;
		case IZQUIERDA:
			err = item_desplazar(nivel, identificador, -1, 0);
		break;
		case DERECHA:
			err = item_desplazar(nivel, identificador, 1, 0);
		break;
	}

	pthread_mutex_unlock(&mutex_mapa);
	return err;
}

int eliminar_tripulante(uint32_t id_tripulante){
	char identificador = asignar_letra(id_tripulante);
	log_info(logger, "MAPA :: Borramos del mapa el tripulante %i con el identificador \"%c\"", id_tripulante, identificador);

	int err = item_borrar(nivel, identificador);

	ASSERT_CREATE(nivel, identificador, err);
	pthread_mutex_unlock(&mutex_mapa);

	return err;
}


