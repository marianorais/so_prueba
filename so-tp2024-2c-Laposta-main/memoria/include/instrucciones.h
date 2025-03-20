#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H


#include <stdlib.h>
#include <stdio.h>
#include <utils/utils.h>
#include <commons/string.h>

#include "../include/init_memoria.h"
#include "../include/particion_dinamica.h"

//----------------------------------Estructuras---------------------------------



//----------------------------------Variables Externs-------------------------



//----------------------------------Prototipos---------------------------------

//void leer_instrucciones(char* nombre_archivo, uint32_t proceso_pid, uint32_t hilo_tid);
char *buscar_instruccion(uint32_t proceso_pid, uint32_t hilo_tid, int program_counter);
void leer_instrucciones_particiones_fijas(char* nombre_archivo, t_hilo* hilo);

#endif 