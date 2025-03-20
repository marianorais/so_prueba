#ifndef CONEXION_H
#define CONEXION_H

#include <stdio.h>
#include <stdlib.h>
#include <utils/utils.h>

#include "../include/init_memoria.h"
#include "../include/protocolo.h"
//----------------------------------Estructuras---------------------------------



//----------------------------------Variables Externs-------------------------



//----------------------------------Prototipos---------------------------------

void iniciar_servidores();
void iniciar_conexiones();
int crear_socket_fs();
//void escuchar_modulos();


#endif 