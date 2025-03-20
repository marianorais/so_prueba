#ifndef PARTICION_DINAMICA_H
#define PARTICION_DINAMICA_H

#include <pthread.h>
#include <commons/string.h>

#include <stdlib.h>
#include <stdio.h>
#include <utils/utils.h>

#include "../include/init_memoria.h"
#include "../include/memoria_usuario.h"


//----------------------------------Estructuras---------------------------------


//----------------------------------Variables Externs-------------------------



//----------------------------------Prototipos---------------------------------

int crear_proceso(uint32_t proceso_pid, uint32_t tamanio_proceso);
int crear_proceso_dinamico(uint32_t proceso_pid, uint32_t tamanio_proceso);

int asignar_memoria(uint32_t proceso_pid, uint32_t tamanio_proceso);
t_particion_dinamica *buscar_first_fit(uint32_t tamanio_proceso);
t_particion_dinamica *buscar_best_fit(uint32_t tamanio_proceso);
t_particion_dinamica *buscar_worst_fit(uint32_t tamanio_proceso);
void dividir_particion(t_particion_dinamica* particion, uint32_t tamanio_proceso);


char* escribir_memoria(uint32_t proceso_pid, uint32_t direccion_fisica, char* valor, uint32_t tamanio_a_escribir);
char* leer_memoria(uint32_t proceso_pid, uint32_t direccion_fisica, uint32_t tamanio_a_leer);

int busco_indice_particion_dinamica_por_PID(uint32_t proceso_pid);
t_particion_dinamica *busco_particion_dinamica_por_PID(uint32_t proceso_pid);
t_miniPCB *busco_proceso_por_PID(uint32_t proceso_pid);
t_hilo *busco_hilo_por_TID(uint32_t hilo_tid, t_miniPCB *proceso);

void unificar_particiones_dinamicas(int indice_particion);
void finalizar_proceso_dinamico(uint32_t proceso_pid);
void finalizar_proceso(uint32_t proceso_pid);

#endif 