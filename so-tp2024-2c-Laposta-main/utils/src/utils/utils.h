#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<commons/config.h>
#include<string.h>
#include<assert.h>
#include<signal.h>
#include <errno.h>
#include<pthread.h>

typedef  enum 
{
//----------------BASICOS--------------------------------
    HANDSHAKE = 1,
    HANDSHAKE_OK,
    OK,

//---------------CPU-KERNEL-------------------

    MUTEX_CREAR,
    MUTEX_BLOQUEAR,
    MUTEX_DESBLOQUEAR,
    HILO_CANCELAR,
    HILO_SALIR,
    HILO_JUNTAR,
    HILO_CREAR,
    PROCESO_CREAR,
    PROCESO_SALIR,
    PROCESO_EJECUTAR,
    IO_EJECUTAR,
    FIN_DE_QUANTUM,
    SOLICITUD_DE_MUTEX_BLOQUEADA,
    
    SEGMENTATION_FAULT,
    
    RESPUESTA_SYSCALL,
    CONTINUA_EJECUTANDO_HILO,
    REPLANIFICACION,

//---------------CPU-MEMORIA-------------------
    SOLICITUD_CONTEXTO,
    SOLICITUD_CONTEXTO_RTA,
    SOLICITUD_INSTRUCCION,
    SOLICITUD_INSTRUCCION_RTA,
    READ_MEMORIA,
    READ_MEMORIA_RTA_OK,
    READ_MEMORIA_RTA_ERROR,
    WRITE_MEMORIA,
    WRITE_MEMORIA_RTA_OK,
    WRITE_MEMORIA_RTA_ERROR,
    DEVOLUCION_CONTEXTO,
    DEVOLUCION_CONTEXTO_RTA_OK,
    DEVOLUCION_CONTEXTO_RTA_ERROR,
    BASE_PARTICION,
    BASE_PARTICION_RTA,


//---------------FILESYSTEM-KERNEL-------------------


//----------------KERNEL-MEMORIA
    INICIAR_PROCESO,
    INICIAR_PROCESO_RTA_OK,
    INICIAR_PROCESO_RTA_ERROR,
    INICIAR_PROCESO_RTA_ERROR_YA_EXISTE,
    INICIAR_PROCESO_RTA_ERROR_SIN_ESPACIO,
    FINALIZAR_PROCESO,
    FINALIZAR_PROCESO_RTA_OK,
    FINALIZAR_PROCESO_RTA_ERROR_NO_EXISTE,
    INICIAR_HILO,
    INICIAR_HILO_RTA_OK,
    INICIAR_HILO_RTA_ERROR_YA_EXISTE,
    FINALIZAR_HILO,
    FINALIZAR_HILO_RTA_OK,
    FINALIZAR_HILO_RTA_ERROR_NO_EXISTE,
    PEDIDO_MEMORY_DUMP,
    PEDIDO_MEMORY_DUMP_RTA_OK,
    PEDIDO_MEMORY_DUMP_RTA_ERROR,

//---------------FILESYSTEM-MEMORIA-------------------
    CREACION_DUMP,
    
}op_code;  

typedef struct {
    uint32_t size; // Tamaño del payload
    uint32_t offset; // Desplazamiento dentro del payload
    void* stream; // Payload
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

void* recibir_buffer(int*, int);
int iniciar_servidor(t_log *logger, const char *name, char *ip, char *puerto);
int esperar_cliente(t_log* logger, const char* name, int socket_servidor);
int crear_conexion(t_log *logger, const char *server_name, char *ip, char *puerto);
t_list* recibir_paquete(int);
int recibir_operacion(int);
void agregar_a_buffer(t_buffer* un_buffer, void* valor, int tamanio);
void eliminar_buffer(t_buffer* un_buffer);
t_paquete* crear_paquete(op_code codigo_operacion);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
bool config_has_all_properties(t_config *cfg, char **properties);
void iterator(char* value);
void terminar_programa(int conexion, t_log* logger, t_config* config);
t_config* iniciar_config(char* path_config, t_log*);
void crear_servidor(t_log* logger);
void handshake_cliente(t_config* config, t_log* logger, int conexion);
#endif /* UTILS_H_ */