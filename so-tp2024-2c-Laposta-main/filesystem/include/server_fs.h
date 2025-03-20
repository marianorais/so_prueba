#ifndef SERVER_FILE_SYSTEM_H
#define SERVER_FILE_SYSTEM_H
#include "../include/init_filesystem.h"
#include"../include/utils_fs.h"

void* crear_servidor_fs(char* ip_file_system);
int server_escuchar_fs(int server_socket, int *global_socket) ;
void procesar_conexion(void *v_args);
#endif /* SERVER_FILE_SYSTEM_H */