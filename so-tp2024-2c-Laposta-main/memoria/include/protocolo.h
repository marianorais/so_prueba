#ifndef PROTOCOLO_H
#define PROTOCOLO_H

#include <utils/utils.h>
#include <pthread.h>
#include <commons/string.h>

#include "../include/init_memoria.h"
#include "../include/utils_memoria.h"
#include "../include/particion_fija.h"
#include "../include/particion_dinamica.h"
#include "../include/instrucciones.h"


//----------------------------------Estructuras---------------------------------



//----------------------------------Variables Externs-------------------------



//----------------------------------Prototipos---------------------------------
void memoria_atender_cpu();
void memoria_atender_kernel(void* arg);
void atender_dump_memory_fs(t_peticion_dump_fs* peticion_fs);



#endif 