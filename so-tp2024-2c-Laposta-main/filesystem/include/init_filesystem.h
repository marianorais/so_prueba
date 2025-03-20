
#ifndef INIT_FILE_SYSTEM_H
#define INIT_FILE_SYSTEM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <commons/string.h>
#include <utils/utils.h>
#include <semaphore.h>
#include <pthread.h>
extern int conexion_memoria;
extern  pthread_mutex_t  mtx_file_system;
typedef struct {
    int fd;
    char *server_name;
} t_procesar_conexion_args;


int checkProperties(char *path_config);

int cargar_configuracion(char *path);

int init(char *path_config);

int hacer_handshake (int socket_cliente);


void cerrar_programa();

extern t_log *logger_file_system;
extern t_config *file_cfg_file_system;

typedef struct
{
    char *PUERTO_ESCUCHA;
    char* MOUNT_DIR;
    int BLOCK_SIZE;
    int BLOCK_COUNT;
    int RETARDO_ACCESO_BLOQUE;
    char *LOG_LEVEL;
} t_config_file_system;

extern t_config_file_system *cfg_file_system;


static t_config_file_system *cfg_file_system_start()
{
    t_config_file_system *cfg = malloc(sizeof(t_config_file_system));
    return cfg;
}


#endif /* INIT_FILE_SYSTEM_H */