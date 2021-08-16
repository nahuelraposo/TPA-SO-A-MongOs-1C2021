#ifndef DISCORDIADOR_SRC_UTILS_DISCORDIADOR_H_
#define DISCORDIADOR_SRC_UTILS_DISCORDIADOR_H_

#include "../../utils/utils/utils.h"
#include "msg_discordiador.h"
#include <math.h>

char* ip_discordiador;
char* puerto_escucha;
char* ip_Mi_RAM_HQ;
char* puerto_Mi_RAM_HQ;
int socket_Mi_RAM_HQ;

char* ip_Mongo_Store;
char* puerto_Mongo_Store;
int socket_Mongo_Store;

char* punto_montaje;
char* archivo_log;

uint32_t grado_multitarea;
uint32_t duracion_sabotaje;
t_posicion* posicion_sabotaje;
uint32_t retardo_ciclo_cpu;

void* planificacion_corto_plazo;
char* algoritmo;
uint32_t quantum;
bool planificacion_habilitada;

t_config* config;
t_log* logger;
pthread_t hiloReceive;

t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_exec;
t_queue* cola_exit;
t_queue* cola_bloq_E_S;
t_queue* cola_bloq_Emergencia;
t_list* lista_bloq_Emergencia;
t_list* lista_exec;
t_list* lista_expulsados;
t_list* lista_tripulantes_plani;

typedef struct {
	t_tripulante* tripulante;
	pthread_mutex_t mutex_solicitud;
	pthread_mutex_t mutex_ejecucion;
//	semt_t semaforo; quiza haya que usar
	bool esta_activo;
} p_tripulante;

pthread_mutex_t mutex_cola_ready;
pthread_mutex_t mutex_cola_exec;
pthread_mutex_t mutex_cola_bloqueados_io;
pthread_mutex_t mutex_cola_bloqueados_sabotajes;
pthread_mutex_t mutex_planificacion_bloqueados_io;
pthread_mutex_t mutex_planificacion_bloqueados_sabotajes;
sem_t semaforo_planificacion;
sem_t semaforo_cola_ready;
sem_t semaforo_cola_exec;
sem_t semaforo_cola_bloqueados_io;

sem_t semaforo_cola_bloqueados_sabotaje;
pthread_mutex_t mutex_sabotajes;
pthread_mutex_t mutex_sabotajes_bloqueados_io;

uint32_t cantidad_patotas;
uint32_t cantidad_tripulantes;

void arrancar_servidor(void);
void serve_client(int*);
void procesar_mensaje_recibido(int, int);
void rafaga_cpu(uint32_t tiempo);
void rafaga();
void rafaga_block_io(uint32_t tiempo);
char* obtener_estado_segun_caracter(char);
int conectar_con_RAM(void);
int conectar_con_MONGO(void);
void bloquear_por_sabotaje();
void desbloquear_por_sabotaje();
bool verificar_sabotaje_cpu();
bool verificar_sabotaje_io();
void detener_tripulantes(void);
bool posicion_mas_cercana(t_posicion*,t_posicion*,t_posicion*);
void planificar_tripulante_para_sabotaje(int);
void regresar_tripulante(t_tripulante* tripulante);

#endif /* DISCORDIADOR_SRC_UTILS_DISCORDIADOR_H_ */
