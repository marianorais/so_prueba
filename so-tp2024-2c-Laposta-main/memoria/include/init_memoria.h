#ifndef INIT_MEMORIA_H
#define INIT_MEMORIA_H


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <utils/utils.h>
#include "semaphore.h"

#include <commons/bitarray.h>
#include <commons/collections/list.h>

//#include "../include/memoria_usuario.h"


//#include "../include/instrucciones.h"


//----------------------------------Estructuras---------------------------------

typedef struct{
    char* PUERTO_ESCUCHA;
    char* IP_FILESYSTEM;
    char* PUERTO_FILESYSTEM;
    uint32_t TAM_MEMORIA;
    char* PATH_INSTRUCCIONES;
    uint32_t RETARDO_RESPUESTA;
    char* ESQUEMA;
    char* ALGORITMO_BUSQUEDA;
    char** PARTICIONES;
    char* LOG_LEVEL;
} t_config_memoria;

typedef struct {
    uint32_t PC;
    uint32_t AX, BX, CX, DX, EX, FX, GX, HX;
} t_registro_hilos;

typedef struct{
    uint32_t pid;
    t_list* hilos;
    uint32_t base;
    uint32_t limite;
} t_miniPCB;



typedef struct{
    uint32_t tid;
    t_registro_hilos registros;
    t_list *lista_de_instrucciones;
}t_hilo;







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







//----------------Serializar/Deserializar------------------
typedef struct{
    uint32_t pid;
    uint32_t tid;
    t_registro_hilos registros;
    uint32_t base;
    uint32_t limite;
} t_m_contexto;


//struct para deserializar/serializar al crear proceso
typedef struct{
    uint32_t pid;                     //pcb del proceso
    uint32_t tamanio_proceso;         //tamanio del proceso
    char *archivo_pseudocodigo;     //nombre del archivo de pseudocodigo
} t_m_crear_proceso;

//struct para deserializar/serializar al crear hilo
typedef struct{
    uint32_t pid;                     //pcb del proceso
    uint32_t tid;                    //tid del hilo
    char *archivo_pseudocodigo;     //nombre del archivo de pseudocodigo
} t_m_crear_hilo;



//struct para deserializar/serializar al leer instruccion
typedef struct{
    uint32_t pid;
    uint32_t tid;
    uint32_t program_counter;
}t_proceso_memoria;


//struct para deserializar/serializar al leer o escribir
typedef struct{
    uint32_t pid;
    uint32_t tid;
    uint32_t direccion_fisica;
    uint32_t tamanio; //siempre son 4 bytes?
    uint32_t valor;
} t_escribir_leer;


//struct para deserializar/serializar para dump
typedef struct{

    /* data */

} t_dump;

//struct para deserializar/serializar al leer o escribir
typedef struct{
    uint32_t tamanio_nombre_archivo;
    char* nombre_archivo;
    uint32_t tamanio_contenido;
    char* contenido;
    uint32_t fd_kernel;
} t_peticion_dump_fs;


//----------------------------------Variables Externs-------------------------


extern int socket_memoria;
extern int socket_cpu;
//extern int socket_kernel;
//extern int socket_filesystem;

extern t_log *logger_memoria;
extern t_config *file_cfg_memoria;
extern t_config_memoria *cfg_memoria;

extern void* memoria_usuario;    
extern t_list* lista_particiones;
extern t_list* lista_particiones_dinamicas;
extern t_list* lista_miniPCBs;
extern uint32_t cantidad_particiones_memoria;
     
extern t_bitarray *bitmap_particiones;
extern t_list* pids_por_bloque;
extern uint32_t tamanio_total_memoria;  
extern char * algoritmo_alocacion;   
extern pthread_mutex_t mutex_memoria;
extern pthread_mutex_t mutex_lista_particiones_dinamicas;
extern pthread_mutex_t mutex_lista_miniPCBs;
extern pthread_mutex_t mutex_bitmap_particiones;
extern pthread_mutex_t mutex_pids_por_bloque;



                    

           
// extern t_list* lista_miniPCBs;  TODO: FIXME: aaaaaaaaaaaaaaaaaa poruqe esta definido en init_memoria.h tambien??
  
    


//----------------------------------Prototipos---------------------------------


//int inicializar_configuraion(char* path_config);
t_config_memoria *cfg_memoria_start();
int init(char *path_config);
int checkProperties(char *path_config);
int cargar_configuracion(char *path_config);

int inicializar_memoria();

void inicializar_memoria_particiones_dinamicas(uint32_t tamanio_memoria);

void inicializar_memoria_particiones_fijas(uint32_t mem_size, uint32_t num_particiones, char* algoritmo);

t_bitarray *crear_bitmap(int entradas);

int redondear_a_multiplo_mas_cercano_de(int base, int valor);

void cerrar_programa();

//void inicializar_proceso(uint32_t pid, uint32_t tamanio_proceso);

//void inicializar_hilo(uint32_t pid, uint32_t tid, char* nombre_archivo);

//void asignar_hilo_a_proceso(t_hilo* hilo, uint32_t pid);

t_list* char_array_to_list(char** array);

void liberar_hilo(t_hilo *hilo);

void liberar_miniPCB(t_miniPCB *miniPCB);

uint32_t buscar_indice_pcb_por_pid(t_list* lista, uint32_t pid);

void eliminar_hilo_de_lista(t_list* lista_procesos, uint32_t pid, uint32_t tid);

uint32_t buscar_indice_hilos_por_tid(t_list* lista, uint32_t tid);

void mostrar_instrucciones(t_list* lista_de_instrucciones);

void mostrar_hilos(t_list* lista_de_hilos);

void mostrar_lista_miniPCB(t_list* lista_miniPCB);

void print_bitarray(t_bitarray *bitarray);

bool existe_proceso_en_memoria(uint32_t pid);

bool existe_hilo_en_memoria(uint32_t pid, uint32_t tid);

uint32_t buscar_tamanio_proceso_por_pid(uint32_t pid);

void eliminar_proceso_de_lista(uint32_t pid);

t_m_contexto* buscar_contexto_en_lista(uint32_t pid, uint32_t tid);

bool actualizar_contexto(t_m_contexto* contexto);

t_miniPCB* obtener_particion_proceso(uint32_t direccion_fisica);

bool write_mem(uint32_t direccion_fisica, uint32_t valor);

bool read_mem(uint32_t direccion_fisica, uint32_t* resultado);

//int crear_proceso(uint32_t proceso_pid, uint32_t tamanio_proceso);

//void finalizar_proceso(uint32_t proceso_pid);

char* generar_nombre_archivo(uint32_t pid, uint32_t tid);


#endif /* MEMORIA_H */