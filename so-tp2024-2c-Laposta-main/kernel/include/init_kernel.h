#ifndef KERNEL_H_
#define KERNEL_H_
#include <utils/utils.h>
//#include "manejo_colas.h"
#include "semaphore.h"
#include <commons/collections/list.h>
#define HILO_MAIN 0
#define SIN_ASIGNAR -1
#define ASIGNADO 1

typedef struct
{
    /* data */
    char *ip_memoria;
    char *puerto_memoria;
    char *ip_cpu;
    char *puerto_dispatch;
    char *puerto_interrupt;
    char *algoritmo_planif;
    int quantum;
    char *log_level;
    int conexion_cpu_dispatch;
    int conexion_cpu_interrupt;

} t_config_kernel;

extern t_config_kernel *config_kernel;
extern t_log *logger_kernel;
extern int pid_AI_global;//contador de pids de procesos
extern int socket_cpu;
typedef enum estado{
    NEW,
    READY,
    EXEC,
    BLOCKED,
    EXIT
} t_estado;
typedef struct 
{
    //atrib minimos requeridos
    int pid;
    int contador_AI_tids;//contador auto-incremental TODO: FIXME: deberia asegurar con mutex??
    t_list *lista_tids;
    t_list *lista_mutex;
    t_estado estado;

    //atrib de creacion 
    int tamanio_proceso;
    char* ruta_pseudocodigo;
    int prioridad_th_main;
} t_pcb;

typedef struct t_tcb{
    /* data */
    int tid;
    int prioridad;
    int pid;
    int tiempo_de_io;
    t_estado estado;
    void * thread_target;//hilo que se espera que termine
    t_list *mutex_asignados;
} t_tcb;

typedef struct
{
    char* recurso;//recurso, identificador del mutex
    t_list* lista_threads_bloquedos;
    t_tcb* thread_asignado;
    int estado;
} t_mutex;

//---------------------- HILOS ------------------------------
typedef struct{
    //TODO: agregar todos los hilos aca
    pthread_t hilo_planif_largo_plazo;
    pthread_t hilo_fifo;
    pthread_t hilo_prioridades;
    pthread_t hilo_colas_multinivel;
    pthread_t hilo_bloqueados;//maneja io,mutex, join
    pthread_t hilo_quantum;
    pthread_t hilo_finalizacion_procesos_memoria;
    
}t_hilos;

extern t_hilos *hilos;

//---------------------------SEMAFOROS------------------------



typedef struct{
    //TODO: agregar todos los semaforos globales aca
    sem_t mutex_lista_new;
    sem_t mutex_lista_ready;
    sem_t mutex_lista_exit;
    sem_t mutex_lista_exec;
    sem_t mutex_lista_blocked;
    sem_t mutex_lista_espera_io;
    sem_t inicializar_planificador;
    sem_t sem_procesos_new;//contado de procesos en new
    sem_t sem_procesos_ready;
    sem_t sem_espacio_liberado_por_proceso;
    sem_t mutex_lista_global_procesos;
    sem_t contador_threads_en_ready;
    sem_t espacio_en_cpu;
    //sem_t mutex_interfaz_io;
    //sem_t contador_tcb_en_io;
    sem_t sem_io_sleep_en_uso;
    sem_t sem_io_solicitud;
    sem_t sem_sleep_io;
    sem_t sem_finalizacion_ejecucion_cpu;


    sem_t mutex_conexion_dispatch;
    sem_t conexion_memoria_dump;
}t_semaforos;
extern t_semaforos* semaforos;

// typedef struct{
//     t_list* threads_en_espera;
//     t_tcb* thread_en_io;
//     bool en_ejecucion;
// }t_io;
// extern t_io* interfaz_io;
//------------------------------LISTAS-------------------------
extern t_list* lista_ready; 
extern t_list* lista_exec;
extern t_list* lista_blocked;
extern t_list* lista_exit;
extern t_list* lista_new;
extern t_list* lista_procesos_global;
extern t_list* lista_espera_io;

void desbloquear_hilos_por_fin_de_hilo(t_tcb* tcb_finalizado);
int conectar_a_memoria();
void generar_conexiones_a_cpu();
void procesar_conexion_interrupt(void *socket);
void procesar_conexion_dispatch();
void iniciar_modulo(char *ruta_config);
void cargar_config_kernel(char *ruta_config);
void process_create(char* ruta_instrucciones,int tamanio_proceso,int prioridad_hilo_main);
t_pcb* crear_pcb(int tam_proceso,char*archivo_instrucciones,int prioridad) ;
t_tcb* crear_tcb(int prio,int pid);
void anadir_tid_a_proceso(t_pcb* pcb);
void enviar_solicitud_espacio_a_memoria(t_pcb* pcb_solicitante,int socket);
int recibir_resp_de_memoria_a_solicitud(int socket_memoria);

void inicializar_hilos_largo_plazo();
void inicializar_hilos_planificador();
void crear_hilo_planificador_fifo();
void crear_hilo_planificador_prioridades();
void crear_hilo_planificador_colas_multinivel();
void *planificar_fifo();
void *planificar_prioridades();
void *planificar_colas_multinivel();
void agregar_a_cola(t_pcb *pcb,t_list* lista,sem_t* sem);
void pasar_new_a_ready();
void pasar_ready_a_exit();
void pasar_new_a_exit();
void inicializar_semaforos();
void pasar_ready_a_execute();
void pasar_execute_a_ready();
void pasar_blocked_a_exit();
void pasar_blocked_a_ready();
void pasar_execute_a_exit();
void pasar_execute_a_blocked();
void* planificar_procesos();
void inicializar_listas();
t_pcb* buscar_proceso_por(int pid_buscado);
void mostrar_pcb(t_pcb* pcb, t_log* logger);
t_tcb* thread_create(char* pseudocodigo,int prio,int pid);
void enviar_thread_a_cpu(t_tcb* tcb_a_ejetucar,int socket_dispatch);
void ejecutar_io(int tiempo);
void *manejar_bloqueados();

void *interrupcion_quantum();
t_mutex* buscar_mutex(char* recurso,int pid);
void asignar_mutex(t_tcb * tcb, t_mutex* mutex);
t_tcb* asignar_mutex_al_siguiente_thread(t_mutex* mutex);
void mutex_lock(char* recurso);
void mutex_create(char* nombre_mutex,int pid_mutex);
void thread_exit(t_tcb* t);
void thread_cancel(int tid,int pid);

void enviar_interrumpir_cpu(t_tcb* tcb, int motivo_interrrupt);
void enviar_a_memoria_creacion_thread(t_tcb* tcb_nuevo,char* pseudo,int socket);
void* enviar_a_memoria_thread_saliente(void* t);
bool quitar_tid_de_proceso(t_tcb *t);
void destruir_tcb(t_tcb* t);
int buscar_indice_de_tid_en_proceso(t_pcb *pcb,int tid);
t_tcb* buscar_en_lista_y_cancelar(t_list* lista,int tid,int pid,sem_t* sem);
t_tcb* buscar_en_lista_tcb(t_list* lista,int tid,int pid,sem_t* sem);
void agregar_a_lista(t_tcb *tcb,t_list* lista,sem_t* sem);
t_tcb* remover_de_lista(t_list* lista,int indice, sem_t* mutex);
t_mutex* quitar_mutex_a_thread(char* recurso,t_tcb* tcb);
void mutex_unlock(char* recurso_unlok,t_tcb* th_unlock);
void thread_join(t_tcb* th_en_exec,int tid_target);
void inicializar_hilo_intefaz_io();
void interfaz_io();
void hilo_sleep_io();
int buscar_indice_de_mayor_prioridad();
void iniciar_quantum();
void mover_procesos(t_list* lista_origen, t_list* lista_destino, sem_t* sem_origen, sem_t* sem_destino, t_estado nuevo_estado);
void mostrar_tcbs(t_list* lista_tcb, t_log* logger);
void memory_dump();
void* atender_dump_memory();
void manejar_interrupcion_fin_quantum();
void enviar_respuesta_syscall_a_cpu(int respuesta);
void cancelar_hilos_asociados(int pid);
void buscar_y_cancelar_tcb_asociado_a_pcb(int pid,t_list* lista,sem_t* sem,t_estado estado_lista);
void inicializar_hilo_verificacion_fin_de_ejecucion();
void verificar_fin_ejecucion_prev_quantum();
void manejo_liberacion_memoria();
void enviar_a_memoria_memory_dump(int pid, int tid, int socket_conexion_memoria);
void* enviar_a_memoria_proceso_saliente(void* t);
#endif /* KERNEL_H_ */