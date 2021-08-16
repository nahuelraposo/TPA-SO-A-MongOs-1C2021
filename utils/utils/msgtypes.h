#ifndef SHARED_SHARED_MSGTYPES_H_
#define SHARED_SHARED_MSGTYPES_H_

#include "paquete.h"
#include <commons/collections/dictionary.h>

typedef enum {
	OK,
	FAIL
} t_respuesta;

typedef struct {
	uint32_t id_tripulante;
	uint32_t id_patota;
	char estado;					// (N/R/E/B)
} t_respuesta_listar_tripulante;

typedef struct {
	uint32_t cantidad_tripulantes;
	t_list* tripulantes;
} t_respuesta_listado_tripulantes;

//---------ESTADO--------//
typedef enum {
	NEW,
	READY,
	BLOCKED_I_O,
	BLOCKED_EMERG,
	EXEC,
	EXIT
} t_estado;

typedef enum {
	ARRIBA,
	ABAJO,
	IZQUIERDA,
	DERECHA
} t_movimiento;

//---------POSICION--------//
typedef struct {
	uint32_t pos_x;
	uint32_t pos_y;
} t_posicion;
//--------ESTRUCTURAS-------//
typedef struct{
	uint32_t pid; //Identificador de la Patota
	uint32_t tareas; //Dirección lógica del inicio de las Tareas
} t_pcb;

typedef struct{
	uint32_t tid; // Identificador del tripulante
	char estado; // Estado del tripulante (N/R/E/B)
	t_posicion* posicion; // Posición del tripulante
	uint32_t prox_instruccion; // Identificador de la próxima instrucción a ejecutar
	uint32_t puntero_pcb; // Dirección lógica del PCB del tripulante
} t_tcb;

//---------TAREAS--------//

typedef struct {
	uint32_t tamanio_tarea;
	char* tarea;
    uint32_t parametro;
	t_posicion* posicion;
    uint32_t tiempo;
} t_tarea;

//---------TRIPULANTE--------//
typedef struct {
	uint32_t id;
	uint32_t id_patota_asociado;
	t_posicion* posicion;
	t_tarea* tarea_act;
	t_estado estado;
	int socket_conexion_RAM;
	int socket_conexion_Mongo;
} t_tripulante;

//---------PATOTA--------//
typedef struct {
	uint32_t id_patota;
    uint32_t cant_tripulantes;
    uint32_t tam_path;
    char* path_tareas;
    t_list* posiciones;
} t_patota;


typedef struct {
	uint32_t id_tripulante;
    t_movimiento direccion;
	uint32_t id_patota_asociado;
	t_posicion* posicion_origen;
	t_posicion* posicion_destino;
} t_mover_hacia;

//--------BITACORA-------//
typedef struct {
	uint32_t id;
	uint32_t tamanio_tarea;
	char* tarea;
	uint32_t parametro;
} t_tarea_Mongo;

typedef struct {
	uint32_t id;
	uint32_t tamanio_tarea;
	char* tarea;
} t_bitacora_tarea_Mongo;



typedef struct {
	char* stream;
	t_dictionary* diccionario;
} t_bloque_FS;

t_patota* deserializar_iniciar_patota(uint32_t);
t_tripulante* deserializar_iniciar_tripulante(uint32_t socket_cliente);
t_respuesta recibir_respuesta(uint32_t);
t_tripulante* deserializar_actualizar_estado(uint32_t socket_cliente);
void enviar_respuesta(t_respuesta, uint32_t);
void serializar_respuesta(t_respuesta, t_buffer*);

t_mover_hacia* deserializar_mover_hacia(uint32_t socket_cliente);;

#endif /* SHARED_SHARED_MSGTYPES_H_ */
