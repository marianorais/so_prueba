#include <utils/hello.h>

#include <stdlib.h>
#include <stdio.h>
#include <utils/hello.h>
#include <utils/utils.h>
#include <commons/config.h>
#include <pthread.h>
#include "../include/main.h"

char *path_config;
char *ip_cpu;
bool fin_ciclo=false;


bool interrupcion_kernel;
instr_t *prox_inst;
t_list* lista_sockets_global;
int conexion_kernel_dispatch = -1;
int conexion_kernel_interrupt = -1;

sem_t sem_valor_instruccion;

sem_t sem_valor_registro_recibido;

sem_t sem_valor_base_particion;
sem_t sem_servidor_creado;
sem_t sem_interrupcion_kernel;
sem_t sem_check_interrupcion_kernel;
sem_t sem_conexion_interrupt_iniciado;
sem_t sem_conexion_dispatch_iniciado;
sem_t sem_esperando_read_write_mem;
sem_t sem_cpu_termino_ciclo;
pthread_mutex_t mutex_proceso_actual;
pthread_mutex_t mutex_proceso_interrumpido_actual;
pthread_mutex_t mutex_interrupcion_kernel;
pthread_t hilo_atender_memoria;
sem_t semaforo_respuesta_syscall;
sem_t semaforo_binario_iniciar_ciclo;
sem_t semaforo_binario_nuevo_proceso;
sem_t semaforo_sincro_contexto_syscall;

int socket_memoria;

uint32_t valor_registro_obtenido;
int rta_resize;


int main(int argc, char *argv[])
{


    path_config = argv[1];
    ip_cpu = argv[2];
    proceso_actual= NULL;
    lista_sockets_global = list_create();


    sem_init(&sem_valor_instruccion, 0, 0);

    sem_init(&sem_valor_registro_recibido, 0, 0);

    sem_init(&sem_valor_base_particion, 0, 0);
    sem_init(&sem_servidor_creado, 0, 0);
    sem_init(&sem_interrupcion_kernel, 0, 0);
    sem_init(&sem_check_interrupcion_kernel, 0, 0);
    sem_init(&sem_conexion_interrupt_iniciado, 0, 0);
    sem_init(&sem_conexion_dispatch_iniciado, 0, 0);
    sem_init(&sem_esperando_read_write_mem, 0, 0);
    sem_init(&sem_esperando_read_write_mem, 0, 0);
    sem_init(&sem_cpu_termino_ciclo, 0, 1);

    sem_init(&semaforo_binario_iniciar_ciclo,0,0);
    sem_init(&semaforo_binario_nuevo_proceso,0,1);
    sem_init(&semaforo_sincro_contexto_syscall,0,0);
    pthread_mutex_init(&mutex_proceso_actual, NULL);
    pthread_mutex_init(&mutex_proceso_interrumpido_actual, NULL);
    pthread_mutex_init(&mutex_interrupcion_kernel, NULL);

    sem_init(&semaforo_respuesta_syscall,0,0);
    
    prox_inst = malloc(sizeof(instr_t));
    printf("Creo prox_inst\n");
    



    printf("iniciando ");
    if (!init(path_config) || !cargar_configuracion(path_config))
    {
        // cerrar_programa();
        printf("No se pudo inicializar entrada salida");
        liberar_memoria();
        return EXIT_FAILURE;
    }

    log_trace(logger_cpu, "empieza el programa");
    socket_memoria = crear_conexion(logger_cpu, "MEMORIA", cfg_cpu->IP_MEMORIA, cfg_cpu->PUERTO_MEMORIA);
    log_trace(logger_cpu, "cree la conexion con memoria");
    if (hacer_handshake(socket_memoria) == HANDSHAKE_OK)
    {
        log_trace(logger_cpu, "Correcto en handshake con memoria");
        sem_post(&sem_servidor_creado);
    }
    else
    {
        log_trace(logger_cpu, "Error en handshake con memoria");
        liberar_memoria();
        return EXIT_FAILURE;
    }
    

   
  
    log_trace(logger_cpu,"conexion memoria %d",socket_memoria);
    pthread_create(&hilo_atender_memoria, NULL, (void *)atender_memoria, &socket_memoria);
    
    log_trace(logger_cpu,"conexion memoria %d",socket_memoria);
    printf("Paso  sem_servidor_creado\n");
    // Obtener tamaño de página




    pthread_t servidor_dispatch;
    pthread_create(&servidor_dispatch, NULL, (void *)crear_servidor_dispatch, ip_cpu);
   // pthread_detach(servidor_dispatch);

    pthread_t servidor_interrupt;
    pthread_create(&servidor_interrupt, NULL, (void *)crear_servidor_interrupt, ip_cpu);
   // pthread_detach(servidor_interrupt);
    log_trace(logger_cpu, "cree los hilos servidor");


    sem_wait(&sem_servidor_creado);
    sem_wait(&sem_conexion_dispatch_iniciado);
    sem_wait(&sem_conexion_interrupt_iniciado);

    pthread_t hilo_ejecutar_ciclo;
    log_trace(logger_cpu,"conexion memoria %d",socket_memoria);
    log_trace(logger_cpu,"voy a crear hilo");
    int result = pthread_create(&hilo_ejecutar_ciclo, NULL, ejecutar_ciclo,NULL);

    pthread_join(servidor_dispatch, NULL);
    pthread_join(servidor_interrupt, NULL);
    pthread_join(hilo_ejecutar_ciclo, NULL);
    pthread_detach(hilo_atender_memoria);
}

void ejecutar_ciclo() {
   

 
    while (1) {
        sem_wait(&semaforo_binario_iniciar_ciclo);
      while(!fin_ciclo){
       
            //pthread_mutex_unlock(&mutex_proceso_actual);
            //t_proceso* proceso_aux=proceso_actual;
                fin_ciclo=ciclo_de_instrucciones( &socket_memoria, proceso_actual, &conexion_kernel_dispatch,&conexion_kernel_interrupt);
            //pthread_mutex_unlock(&mutex_proceso_actual);                  
      }
       sem_post( &semaforo_binario_nuevo_proceso);
 }

    
}

void liberar_memoria()
{
   

    // Liberar semáforos
    sem_destroy(&sem_valor_instruccion);

    sem_destroy(&sem_valor_base_particion);

    /*free(prox_inst);*/

   

    // Liberar strings
   /* free(path_config);
    free(ip_cpu);
    free(valor_registro_obtenido);
*/

    // Liberar config y logger(declarados en init_cpu)
    cerrar_programa();
}
