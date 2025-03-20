#ifndef ESTRUCTURAS_FS_H
#define ESTRUCTURAS_FS_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utils/utils.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "../include/init_filesystem.h"
#include"../include/utils_fs.h"

/////////////////////////////////////////////////INICIAR FS////////////////////////////////////
///////////////////////////////////////////////PETICIONES//////////////////////////////////////
//////////////////////////////////////////////ESTRUCTURAS//////////////////////////////////////


//BITMAP
extern FILE* archivo_bitmap;
extern void* bitmap;
extern t_bitarray* bitarray;  // estructura para el manejo del bitmap
extern  bitmap_size_in_bytes;
extern block_count;
extern fd_bitmap;

// ARCHIVO DE BLOQUES
extern FILE *archivo_bloques; //
extern int fd_archivo_bloques;


// FCB ARCHIVO DE METADATA
extern t_config *file_fcb;

typedef struct
{
    char* nombre_archivo;
    uint32_t  tamanio_archivo;
    uint32_t  primer_bloque; 
} t_FCB;

extern t_FCB *fcb;


char* uint32_to_string (uint32_t number);


///////////////////////////////////////////////FUNCIONALIDADES//////////////////////////////////////
void dumpear(t_dumped* dumped, int socket_cliente);
t_list* asignar_bloques(uint32_t tamanio, char* nombre_archivo);
void escribir_bloque_punteros (uint32_t* lista_bloques, char* nombre_archivo);
void escribir_bloque_datos (int numero_bloque, int tamanio_escritura, char *datos_escribir, char* nombre_archivo);
void grabar_bloques(t_list* lista_bloques, char *datos_escribir, char* nombre_archivo ,int tamanio_archivo);
//Devuelve la posicion del primer bit libre que encuentra.
uint32_t encontrar_bit_libre(t_bitarray* bitarray);


// devuelve si hay espacio disponible no importa si está contiguo
bool hay_espacio_total_disponible(int espacio_necesario);

//inicia la filesystem 
void iniciar_fs (); 
int crear_bitmap (char * path_archivo_bitmap);
// sincroniza el bitarray en memoria y con el archivo fisico.
void sincronizar_bitmap ();
//crea el archivo de bloques fisico en la ruta especificada.
int crear_archivo_bloques (char * path_archivo_bloques, int block_size, int block_count) ;
void cerrar_bitmap();
//guarda la estructura metadata en un archivo fisico terminado en txt
void persistir_metadata(t_dumped *dumped, int primer_bloque ); 
//devuelve la posicion de un bit libre en un bit array
uint32_t encontrar_bit_libre(t_bitarray* bitarray_in); 
int bloques_libres();
void imprimir_estado_bitarray() ;
uint32_t dividir_redondear_hacia_arriba(uint32_t numerador, uint32_t denominador);
//para liberar la memoria ocupada por un fcb
void free_t_FCB(t_FCB* fcb); 
void enviar_resultado_memoria(op_code codigo_operacion, int socket_memoria);
#endif //ESTRUCTURAS_FS_H