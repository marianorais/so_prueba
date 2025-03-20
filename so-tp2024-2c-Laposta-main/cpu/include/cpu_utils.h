#ifndef CPU_UTILS_H
#define CPU_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include "../include/init_cpu.h"
#include <utils/utils.h>
#include <signal.h>
#include <utils/hello.h>
#include<commons/config.h>
#include <pthread.h>
#include <math.h>

typedef enum
{
    PC,
	AX,
	BX,
	CX,
	DX,
    EX,
    FX,
    GX,
    HX,
    base,
    limite,
    REG_NO_ENC
}registros;

//ciclo de instrccciones 

bool ciclo_de_instrucciones(int *conexion_mer, t_proceso *proceso, int *socket_dispatch,int *socket_interrupt);
instr_t* fetch(int conexion, t_proceso* proceso);
void pedir_instruccion(t_proceso* proceso,int conexion);
tipo_instruccion decode(instr_t* instr, int conexion_memo);
void execute(instr_t* inst,tipo_instruccion tipo_inst, t_proceso* proceso, int conexion,  int socket_dispatch, int socket_interrupt);
void check_interrupt( int conexion_kernel);


//instrucciones

void set(char* registro, char* valor, t_proceso* proceso);
void read_mem(char* registro_datos, char* registro_direccion, t_proceso* proceso, int conexion);
void write_mem(char* registro_direccion, char* registro_datos, t_proceso* proceso, int conexion);
void sum(char* registro_destino, char* registro_origen, t_proceso* proceso);
void sub(char* registro_destino, char* registro_origen, t_proceso* proceso);
void jnz(char* registro, char* inst_char, t_proceso* proceso);
void loguear(char* registro);

// syscalls

void enviar_dump_memory_a_kernel(int socket_dispatch);
void enviar_io_a_kernel(char* tiempo ,int socket_dispatch);
void enviar_process_create_a_kernel(char* nombre_pseudocodigo, char* tamanio_proceso, char* prioridad_hilo, int socket_dispatch);
void enviar_thread_create_a_kernel(char* nombre_pseudocodigo, char* prioridad_hilo, int socket_dispatch);
void enviar_thread_join_a_kernel(char* tid ,int socket_dispatch);
void enviar_thread_cancel_a_kernel(char* tid ,int socket_dispatch);
void enviar_mutex_create_a_kernel(char* recurso, int conexion_kernel);
void enviar_mutex_lock_a_kernel(char* recurso, int conexion_kernel);
void enviar_mutex_unlock_a_kernel(char* recurso, int conexion_kernel);
void enviar_process_exit_a_kernel(int conexion_kernel);
void enviar_thread_exit_a_kernel(int conexion_kernel);


void* crear_servidor_dispatch(char* ip_cpu);//
void* crear_servidor_interrupt(char* ip_cpu);//
registros identificarRegistro(char* registro);
uint32_t obtenerValorActualRegistro(registros id_registro, t_proceso* proceso);


//devuelve la direccion fisica
uint32_t mmu(uint32_t direccion_logica, t_proceso*  proceso, int conexion, int conexion_kernel_dispatch);

char* uint32_to_string(uint32_t number);
//char* concatenar_cadenas(const char* str1, const char* str2);
uint32_t string_a_uint32(const char* str);

void pedir_valor_a_memoria(uint32_t dir_fisica, uint32_t pid, uint32_t tid, int conexion);

tipo_instruccion str_to_tipo_instruccion(const char *str);



void pedir_contexto_a_memoria(int conexion, int pid);

void limpiarCadena(char* cadena);

void generar_interrupcion_a_kernel(int conexion);
void enviar_contexto_a_memoria(t_proceso* proceso, int conexion);
void solicitar_contexto_a_memoria(t_proceso* proceso, int conexion);
void enviar_segfault_a_kernel(t_proceso* proceso,int conexion_kernel_dispatch);
#endif //CPU_UTILS_H