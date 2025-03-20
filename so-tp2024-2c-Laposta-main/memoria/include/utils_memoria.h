#ifndef UTILS_MEMORIA_H
#define UTILS_MEMORIA_H



#include "../include/init_memoria.h"





//----------------------------------Prototipos---------------------------------

t_m_crear_proceso* deserializar_iniciar_proceso(t_list*  lista_paquete );
void enviar_respuesta_iniciar_proceso(t_m_crear_proceso* crear_proceso ,int socket_kernel, op_code cod_ope);
t_m_crear_hilo* deserializar_iniciar_hilo(t_list*  lista_paquete );
void enviar_respuesta_iniciar_hilo(t_m_crear_hilo* crear_hilo ,int socket_kernel, op_code cod_ope);
uint32_t deserializar_finalizar_proceso(t_list*  lista_paquete );
void enviar_respuesta_finalizar_proceso(uint32_t pid_proceso_a_finalizar ,int socket_kernel, op_code cod_ope);
uint32_t deserializar_finalizar_hilo(t_list*  lista_paquete );
void enviar_respuesta_finalizar_hilo(uint32_t pid_proceso_a_finalizar ,uint32_t tid_proceso_a_finalizar,int socket_kernel,op_code cod_ope);

//t_m_contexto* deserializar_contexto(t_list*  lista_paquete );
void enviar_respuesta_contexto(t_m_contexto* pcbproceso, int socket_cpu);
t_proceso_memoria* deserializar_solicitud_instruccion(t_list*  lista_paquete );
void enviar_respuesta_instruccion(char* proxima_instruccion ,int socket_cpu);
t_escribir_leer* deserializar_read_memoria(t_list*  lista_paquete );
void enviar_respuesta_read_memoria(uint32_t pid, uint32_t respuesta_leer, int socket_cpu, op_code cod_ope) ;
t_escribir_leer* deserializar_write_memoria(t_list*  lista_paquete);
void enviar_respuesta_write_memoria(uint32_t pid, int socket_cliente, op_code cod_ope);
void enviar_respuesta_actualizar_contexto(t_m_contexto* contexto ,int socket_cpu, op_code cod_ope);
void enviar_creacion_memory_dump(uint32_t tamanio_nombre_archivo, char* nombre_archivo ,uint32_t tamanio_contenido,char* contenido, int socket_fs);
void deserializar_contexto(t_m_contexto* contexto,t_list*  lista_paquete );
void enviar_confirmacion_memory_dump_a_kernel(op_code cod_ope,int socket_kernel);


#endif /* MEMORIA_H */