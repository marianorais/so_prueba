#ifndef MEMORIA_USUARIO_H
#define MEMORIA_USUARIO_H

#include <pthread.h>
#include <commons/string.h>
#include <commons/bitarray.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <utils/utils.h>
#include <math.h>


#include "../include/init_memoria.h"
#include "../include/instrucciones.h"
#include "../include/particion_dinamica.h"


//----------------------------------Estructuras---------------------------------
/*
// Estructura para las particiones dinamicas

typedef struct{
    uint32_t pid;                   // Id del proceso al que pertenece la particion
    uint32_t tid;                   // Id del hilo al que pertenece la particion
    uint32_t inicio;                // Posición de inicio en el espacio de memoria
    uint32_t tamanio;               // Tamaño de la partición
    bool ocupado;                   // Estado de la partición: libre u ocupada
    //t_particion_dinamica* siguiente;  // Puntero a la siguiente partición (lista enlazada)
} t_particion_dinamica;

//struct para controlar los pids asociados a cada bloque en particiones fijas
typedef struct{
    uint32_t pid;                     //pcb del proceso
    uint32_t bloque;     //bloque de memoria en donde se encuentra el proceso
} t_pid_por_bloque;
*/

//----------------------------------Variables Externs-------------------------
/*
extern void* memoria_usuario;                        
extern t_list* lista_particiones;
extern t_list* lista_particiones_dinamicas;             
// extern t_list* lista_miniPCBs;  TODO: FIXME: aaaaaaaaaaaaaaaaaa poruqe esta definido en init_memoria.h tambien??
extern uint32_t tamanio_total_memoria;  
extern char * algoritmo_alocacion;     
extern t_list* pids_por_bloque;     
*/

//----------------------------------Prototipos---------------------------------
//int redondear_a_multiplo_mas_cercano_de(int base, int valor);

void inicializar_proceso(uint32_t pid, uint32_t tamanio_proceso);

int crear_proceso_fijas(uint32_t tamanio_proceso, t_list* lista_particiones, uint32_t proceso_pid);

void finalizar_proceso_fijas(uint32_t pid);

//void inicializar_memoria_particiones_fijas(uint32_t mem_size, uint32_t num_particiones, char* algoritmo);

//void inicializar_memoria_particiones_dinamicas(size_t mem_size, char* algoritmo);

void* alocar_memoria(uint32_t size);

//int crear_proceso(uint32_t tam_proceso, t_list* lista_de_particiones, uint32_t pid);

//t_bitarray *crear_bitmap(int entradas);

//void finalizar_proceso(void* direccion_proceso);

//uint32_t read_mem(uint32_t direccion_fisica);

//bool write_mem(uint32_t direccion_fisica, uint32_t valor);

void uint32_to_string(uint32_t num, char *str, size_t size);

uint32_t buscar_indice_bloque_por_pid(t_list* lista, uint32_t pid);

void print_pid_por_bloque(void* element);

void print_lista_pid_por_bloque(t_list* lista);

uint32_t calcular_base_proceso_fijas(uint32_t bloque, t_list* particiones);

void inicializar_hilo(uint32_t pid, uint32_t tid, char* nombre_archivo);

void asignar_hilo_a_proceso(t_hilo* hilo, uint32_t pid);


#endif 