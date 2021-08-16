#ifndef MI_RAM_HQ_SRC_MI_RAM_HQ_H_
#define MI_RAM_HQ_SRC_MI_RAM_HQ_H_

#include "../../utils/utils/utils.h"
#include "utils_ram.h"
#include "memoria.h"

#define ARCHIVO_CONFIG "../Mi-RAM-HQ.config"

void iniciar_Mi_RAM_HQ(void);
t_log* iniciar_logger_RAM(char* archivo_log, char* archivo_c);
void leer_config(void);
void finalizar_mi_ram(int signum);

#endif /* MI_RAM_HQ_SRC_MI_RAM_HQ_H_ */
