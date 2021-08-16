#ifndef MI_RAM_HQ_SRC_MEMORIA_H_
#define MI_RAM_HQ_SRC_MEMORIA_H_

#include "utils_ram.h"
#include "mapa.h"
#include <sys/mman.h>
#include <fcntl.h>

#define TAMANIO_PCB 8
#define TAMANIO_TCB 21
#define TAMANIO_OFFSET 1000

#define TAMANIO_INICIAL_TABLA_PAGINACION 256

pthread_t hiloReceiveMapa;

void iniciar_memoria(void);
void preparar_memoria(void);
void crear_tabla(uint32_t id_patota);
uint32_t cantidad_paginas_pedidas();
t_pagina* crear_pagina();
t_tabla_paginas* crear_tabla_paginacion(uint32_t id_patota);
uint32_t obtener_direccion_pcb(uint32_t id_patota);
uint32_t obtener_direccion_tarea(uint32_t id_patota, uint32_t offset);
t_pcb* crear_pcb(t_patota* patota);
t_tcb* crear_tcb(t_tripulante* tripulante);
void preparar_memoria_para_esquema_de_segmentacion(void);
void preparar_memoria_para_esquema_de_paginacion(void);
FILE* crear_archivo_dump();
void dump_memoria_principal();
t_pagina* obtenerPaginaAsociada(t_marco* marco);

void cargar_memoria_patota(t_patota* patota);

uint32_t dar_direccion_logica(uint32_t nro_segmento, uint32_t offset);
uint32_t dar_numero_particion_direccion_logica(uint32_t dir_logica);
uint32_t dar_offset_direccion_logica(uint32_t dir_logica);
t_tarea* obtener_tarea_memoria(t_tripulante* tripulante);

void escribir_en_memoria(void* informacion, uint32_t patota_asociada, e_tipo_dato tipo_dato);
void modificar_memoria(void* informacion, uint32_t id_patota, e_tipo_dato tipo_dato);
void mover_tripulante_memoria(t_mover_hacia* mover_hacia);
void* leer_memoria(uint32_t id, uint32_t id_patota, e_tipo_dato tipo_dato);

t_segmento* buscar_segmento_id(uint32_t id, uint32_t id_patota, e_tipo_dato tipo_dato);
void* leer_memoria_segmentacion(t_segmento* segmento);
void modificar_memoria_segmentacion(t_buffer* buffer, uint32_t patota_asociada, e_tipo_dato tipo_dato);
void escribir_en_memoria_paginacion(t_buffer* buffer, uint32_t id_patota_asociada, e_tipo_dato tipo_dato);
void escribir_en_memoria_segmentacion(t_buffer* buffer, uint32_t patota_asociada, e_tipo_dato tipo_dato);
t_segmento* dar_nuevo_segmento(t_segmento* segmento, uint32_t size);
void subir_segmento_memoria(t_segmento* segmento, void* stream);
void subir_segmento_libre(t_segmento* segmento);
t_tabla_segmentos* dar_tabla_segmentos(uint32_t id_patota);
void subir_tabla_segmento(t_segmento* segmento, uint32_t id_patota, uint32_t id_tripulante, e_tipo_dato tipo_dato);
t_segmento* buscar_segmento_libre(uint32_t espacio_requerido);
void verificar_tabla_segmentos_vacia(t_tabla_segmentos* tabla);

t_segmento* sacar_de_tabla_segmentacion(uint32_t id, uint32_t patota_asociada, e_tipo_dato tipo_dato);
void liberar_segmento(t_segmento* segmento);
void sacar_de_memoria(uint32_t id, uint32_t patota_asociada, e_tipo_dato tipo_dato);
void expulsar_tripulante(t_tripulante* tripulante);

t_list* entradas_segun_patota(uint32_t idPatota);
t_marco* buscar_marco_libre();
void agregar_a_lista_de_marcos_en_paginas(t_pagina* pagina);
void asignar_marco(t_pagina* pagina);
void agregar_paginas(t_tabla_paginas* tabla, uint32_t cantidad);
uint32_t crear_archivo_swap();
void llenar_archivo(int fd, uint32_t tamanio);
uint32_t buscar_lugar_en_swap();
void inicializar_bitmap_swap();
int cantidad_de_marcos_pedidos_swap(int cantidadDeMarcos);
t_tabla_paginas* dar_tabla_paginas(uint32_t id_patota);
t_pagina* obtener_pagina_libre_tabla(t_tabla_paginas* tabla);
void generar_proceso_de_pase_a_swap(t_marco* marcoALimpiar);
t_marco* traer_pagina_con_marco_asignado(t_pagina* pagina, uint32_t id_patota);
t_marco_en_swap* buscar_marco_en_swap(t_pagina* pagina, uint32_t id_patota);
t_marco_en_swap* asignar_marco_en_swap_y_sacar_de_memoria(t_pagina* pagina, uint32_t id_patota_asociada);

bool verificar_paginas_en_memoria(t_tabla_paginas* tabla);
void traer_paginas_a_memoria(t_tabla_paginas* tabla);
void realizar_proceso_de_verificacion_de_paginas_en_memoria(t_tabla_paginas* tabla);
void modificar_memoria_estado_tripulante(t_tripulante* tripulante,char nuevo_estado);
t_tabla_paginas* obtener_tabla_paginas_con_id_patota(uint32_t id_patota);
t_pagina* obtener_pagina_con_marco(t_marco* marco);
t_tabla_paginas* obtener_tabla_paginas_con_marco(t_marco* marco);
t_marco_en_swap* buscar_marco_libre_en_swap();

t_list* buscar_paginas_id(uint32_t id, uint32_t id_patota, e_tipo_dato tipo_dato);
t_list* obtener_paginas_asignadas(t_tabla_paginas* tabla, uint32_t id_tripulante, e_tipo_dato tipo_dato);
t_asociador_pagina* dar_asociador_pagina(t_pagina* pagina, uint32_t id_tripulante, e_tipo_dato tipo_dato);
t_asociador_pagina* quitar_asociador_pagina(t_pagina* pagina, uint32_t id_tripulante, e_tipo_dato tipo_dato);

void escribir_en_memoria_paginacion(t_buffer* buffer, uint32_t id_patota_asociada, e_tipo_dato tipo_dato);
void modificar_memoria_paginacion(t_buffer* buffer, uint32_t patota_asociada, e_tipo_dato tipo_dato);
void* leer_memoria_paginacion(uint32_t id, uint32_t id_patota, e_tipo_dato tipo_dato);

void* convertir(char* algoritmo_nombre);
void seleccionar_victima_LRU(void);
void seleccionar_victima_CLOCK(void);
bool hay_marcos_libres(void);
t_marco* buscar_marco(uint32_t marco);
t_marco* obtenerMarco(void);
bool ambos_bits_apagados(t_marco* entrada);
bool bit_uso_apagado(t_marco* entrada);
bool bit_uso(t_marco* marco);
int indice_elemento(t_list* lista, void* elemento);
int cantidad_de_marcos;
int cantidad_de_marcos_swap;

#endif /* MI_RAM_HQ_SRC_MEMORIA_H_ */
