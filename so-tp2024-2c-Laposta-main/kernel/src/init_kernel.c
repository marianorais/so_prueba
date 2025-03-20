#include <../include/init_kernel.h>

t_config_kernel *config_kernel;
t_log *logger_kernel;
t_semaforos *semaforos;
t_hilos *hilos;
int pid_AI_global;

//t_io *interfaz_io;
int socket_cpu;
void iniciar_modulo(char *ruta_config)
{   
    //logger_kernel = log_create("logs_kernel.log", "KERNEL", true, LOG_LEVEL_TRACE);
    cargar_config_kernel(ruta_config);
    pid_AI_global = 0;
    semaforos = malloc(sizeof(t_semaforos));
    hilos = malloc(sizeof(t_hilos));
    inicializar_listas();
    inicializar_semaforos();
    inicializar_hilos_planificador(); // corto_plazo
    inicializar_hilos_largo_plazo();
    inicializar_hilo_intefaz_io();
    //inicializar_hilo_verificacion_fin_de_ejecucion();
}
void cargar_config_kernel(char *ruta_config)
{
    t_config *config = iniciar_config(ruta_config, logger_kernel);
    config_kernel = malloc(sizeof(t_config_kernel));
    config_kernel->algoritmo_planif = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    config_kernel->ip_cpu = config_get_string_value(config, "IP_CPU");
    config_kernel->ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    config_kernel->puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    logger_kernel = log_create("logs_kernel.log", "KERNEL", true, log_level_from_string(strdup(config_get_string_value(config, "LOG_LEVEL"))));
    config_kernel->log_level = config_get_string_value(config, "LOG_LEVEL");
    config_kernel->puerto_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    config_kernel->puerto_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    config_kernel->quantum = config_get_int_value(config, "QUANTUM");

    log_trace(logger_kernel, "configuracion cargada");
}

// void inicializar_hilo_verificacion_fin_de_ejecucion(){
//     pthread_t hilo_verificacion_fin_ejecucion;
//     pthread_create(&hilo_verificacion_fin_ejecucion, NULL, (void *)verificar_fin_ejecucion_prev_quantum, NULL);
//     pthread_detach(hilo_verificacion_fin_ejecucion);
// }


void inicializar_semaforos()
{
    sem_init(&(semaforos->mutex_lista_new), 0, 1);
    sem_init(&(semaforos->mutex_lista_ready), 0, 1);
    sem_init(&(semaforos->mutex_lista_exit), 0, 1);
    sem_init(&(semaforos->mutex_lista_exec), 0, 1);
    sem_init(&(semaforos->mutex_lista_blocked), 0, 1);
    sem_init(&(semaforos->mutex_lista_espera_io), 0, 1);
    sem_init(&(semaforos->inicializar_planificador), 0, 0);
    sem_init(&(semaforos->sem_procesos_new), 0, 0);
    sem_init(&(semaforos->mutex_lista_global_procesos), 0, 1);
    sem_init(&(semaforos->espacio_en_cpu), 0, 1);
    sem_init(&(semaforos->contador_threads_en_ready), 0, 0);
    //sem_init(&(semaforos->mutex_interfaz_io), 0, 1);
    //sem_init(&(semaforos->contador_tcb_en_io),0,0);
    sem_init(&(semaforos->sem_io_sleep_en_uso),0,0); //revisar aca si empieza con 1 0 0
    sem_init(&(semaforos->sem_io_solicitud),0,0);
    sem_init(&(semaforos->sem_sleep_io),0,0);
    sem_init(&(semaforos->sem_finalizacion_ejecucion_cpu),0,0);

    sem_init(&(semaforos->mutex_conexion_dispatch),0,1);
    sem_init(&(semaforos->conexion_memoria_dump),0,0);
}

int conectar_a_memoria()
{
    int conexion_memoria = crear_conexion(
        logger_kernel, "MEMORIA", config_kernel->ip_memoria, config_kernel->puerto_memoria);
    return conexion_memoria;
}
void generar_conexiones_a_cpu()
{
    pthread_t conexion_cpu_dispatch_hilo;
    pthread_t conexion_cpu_interrupt_hilo;
    log_trace(logger_kernel, "ip cpu:%s",config_kernel->ip_cpu);

    log_trace(logger_kernel, "ip cpu:%s",config_kernel->puerto_dispatch);
    // conecta a por dispatch a cpu
    config_kernel->conexion_cpu_dispatch = crear_conexion(
        logger_kernel, "CPU", config_kernel->ip_cpu, config_kernel->puerto_dispatch);
        if(config_kernel->conexion_cpu_dispatch>0)
            log_trace(logger_kernel, "conectado a cpu");


    // pthread_create(&conexion_cpu_dispatch_hilo, NULL, (void *)procesar_conexion_dispatch, (void *)&(config_kernel->conexion_cpu_dispatch));
     pthread_create(&conexion_cpu_dispatch_hilo, NULL, (void *)procesar_conexion_dispatch, NULL);
    pthread_detach(conexion_cpu_dispatch_hilo);
    

    // conecta a por interrupt a cpu
    config_kernel->conexion_cpu_interrupt = crear_conexion(logger_kernel, "CPU", config_kernel->ip_cpu, config_kernel->puerto_interrupt);
    // pthread_create(&conexion_cpu_interrupt_hilo, NULL, (void*) procesar_conexion_interrupt, (void *)&(config_kernel->conexion_cpu_interrupt));
    // pthread_detach(conexion_cpu_interrupt_hilo);
}

// void iniciar_interfaz_io()
// {
//     t_io *interfaz_io = malloc(sizeof(t_io));
//     interfaz_io->en_ejecucion = false;
//     interfaz_io->thread_en_io = NULL;
//     interfaz_io->threads_en_espera = list_create();
// }
void enviar_respuesta_syscall_a_cpu(int respuesta){
    log_trace(logger_kernel,"enviada respuesat de syscall");
    t_paquete *paquete=crear_paquete(RESPUESTA_SYSCALL);
    agregar_a_paquete(paquete, &respuesta,sizeof(int) );
    enviar_paquete(paquete,config_kernel->conexion_cpu_interrupt);
    eliminar_paquete(paquete);
}
void procesar_conexion_dispatch()
{
    // TODO: operaciones a ejecutar
    while (1)
    {

        int fd_conexion_cpu = config_kernel->conexion_cpu_dispatch;
        int operacion = recibir_operacion(fd_conexion_cpu);
        log_trace(logger_kernel, "se recibio el codigo de operacion: %d", operacion);
        switch (operacion)
        {
        case SEGMENTATION_FAULT: // Sigo el mismo comportamiento que PROCESO_SALIR
            sem_post (&(semaforos->sem_finalizacion_ejecucion_cpu));
            t_list *params_proceso_seg_fault = recibir_paquete(fd_conexion_cpu);

            sem_wait(&(semaforos->mutex_lista_exec));
            t_tcb *proceso_a_finalizar_seg_fault = (t_tcb *)list_get(lista_exec, 0);
            sem_post(&(semaforos->mutex_lista_exec));

            log_info(logger_kernel, "## (<%d>:<%d>) - Solicitó syscall: SEGMENTATION_FAULT",proceso_a_finalizar_seg_fault->pid, proceso_a_finalizar_seg_fault->tid);

            cancelar_hilos_asociados(proceso_a_finalizar_seg_fault->pid);

            pasar_execute_a_exit();

            enviar_respuesta_syscall_a_cpu(REPLANIFICACION);
            sem_post(&(semaforos->espacio_en_cpu));
            list_destroy_and_destroy_elements(params_proceso_seg_fault, free);

            break;
        case PROCESO_CREAR:
            t_list *params_para_creacion = recibir_paquete(fd_conexion_cpu);
            char *ruta_codigo = list_get(params_para_creacion, 0);
            int tamanio_proceso = *((int *)list_get(params_para_creacion, 1));
            int prioridad_main = *((int *)list_get(params_para_creacion, 2));

            process_create(ruta_codigo, tamanio_proceso, prioridad_main);

            sem_wait(&(semaforos->mutex_lista_exec));
            t_tcb* sigue_ejecutando=(t_tcb*)list_get(lista_exec,0);
            sem_post(&(semaforos->mutex_lista_exec));

            log_info(logger_kernel, "## (<%d>:<%d>) - Solicitó syscall: PROCESO CREAR", sigue_ejecutando->pid, sigue_ejecutando->tid);
            
            //enviar_thread_a_cpu(sigue_ejecutando,fd_conexion_cpu);
            enviar_respuesta_syscall_a_cpu(CONTINUA_EJECUTANDO_HILO);
            list_destroy_and_destroy_elements(params_para_creacion, free);
            break;
        case PROCESO_SALIR:
         t_list *params_proceso_salir = recibir_paquete(fd_conexion_cpu);
                sem_post (&(semaforos->sem_finalizacion_ejecucion_cpu));
                //TODO: implementar la finalizacion de proceso: ELIMINACION DE HILOS RESTANTES (SI LOS HAY)

                sem_wait(&(semaforos->mutex_lista_exec));
                t_tcb *proceso_a_finalizar =(t_tcb*) list_get(lista_exec, 0);
                sem_post(&(semaforos->mutex_lista_exec));

                log_info(logger_kernel, "## (<%d>:<%d>) - Solicitó syscall: PROCESO SALIR:", proceso_a_finalizar->pid, proceso_a_finalizar->tid);

                cancelar_hilos_asociados (proceso_a_finalizar->pid);
                
                pasar_execute_a_exit(); //Se autocancela
                //thread_exit(proceso_a_finalizar); //revisar esto, parace estar de mas
                enviar_respuesta_syscall_a_cpu(REPLANIFICACION);
                sem_post(&(semaforos->espacio_en_cpu));
                //sem_post(&(semaforos->sem_espacio_liberado_por_proceso));
                list_destroy_and_destroy_elements(params_proceso_salir, free);
        	break;
        case HILO_CREAR:
            t_list *params_thread = recibir_paquete(fd_conexion_cpu);
            char *codigo_th = list_get(params_thread, 0);
            int prioridad_th = *((int *)list_get(params_thread, 1));

            sem_wait(&(semaforos->mutex_lista_exec));
            int pid_asociado=((t_tcb *)list_get(lista_exec, 0))->pid;
            int tid_asociado=((t_tcb *)list_get(lista_exec, 0))->tid;
            sem_post(&(semaforos->mutex_lista_exec));

            log_info(logger_kernel, "## (<%d>:<%d>) - Solicitó syscall: INICIAR HILO", pid_asociado, tid_asociado);
            
            t_tcb* nuevo_tcb=thread_create(codigo_th, prioridad_th, pid_asociado);
            agregar_a_lista(nuevo_tcb,lista_ready,&(semaforos->mutex_lista_ready));
            sem_post(&(semaforos->contador_threads_en_ready));
            log_info(logger_kernel, "## (<%d>:<%d>) Se crea el Hilo - Estado: READY",nuevo_tcb->pid,nuevo_tcb->tid);
 
           /* sem_wait(&(semaforos->mutex_lista_exec));
            sigue_ejecutando=(t_tcb*)list_get(lista_exec,0);
            sem_post(&(semaforos->mutex_lista_exec));
            
            enviar_thread_a_cpu(sigue_ejecutando,fd_conexion_cpu);*/
             enviar_respuesta_syscall_a_cpu(CONTINUA_EJECUTANDO_HILO);
             list_destroy_and_destroy_elements(params_thread,free);
            break;
        case MUTEX_CREAR: // recurso,pid
            t_list *params_mutex_create = recibir_paquete(fd_conexion_cpu);
            int pid_mutex = *((int *)list_get(params_mutex_create, 0));
            char *nombre_mutex = list_get(params_mutex_create, 1);
            
            sem_wait(&(semaforos->mutex_lista_exec));
            sigue_ejecutando=(t_tcb*)list_get(lista_exec,0);
            sem_post(&(semaforos->mutex_lista_exec));

            log_info(logger_kernel, "## (<%d>:<%d>) - Solicitó syscall: MUTEX_CREATE: %s", sigue_ejecutando->pid, sigue_ejecutando->tid, nombre_mutex);
           
            mutex_create(nombre_mutex, pid_mutex);

            //enviar_thread_a_cpu(sigue_ejecutando,fd_conexion_cpu);
            enviar_respuesta_syscall_a_cpu(CONTINUA_EJECUTANDO_HILO);
            //list_destroy_and_destroy_elements(params_mutex_create,free);*

            break;
        case IO_EJECUTAR: // PID, TID, tiempo de io en milisegundos
            sem_post (&(semaforos->sem_finalizacion_ejecucion_cpu));
            
            t_list *params_io = recibir_paquete(fd_conexion_cpu);
            int tiempo_io = *((int *)list_get(params_io, 2));
            
            sem_wait(&(semaforos->mutex_lista_exec));
            t_tcb *el_que_solicito_un_tiempo = list_get(lista_exec,0);
            sem_post(&(semaforos->mutex_lista_exec));

            log_info(logger_kernel, "## (<%d>:<%d>) - Solicitó syscall: IO", el_que_solicito_un_tiempo->pid, el_que_solicito_un_tiempo->tid);

            ejecutar_io(tiempo_io);
            //la respuesta a la syscall es enviar otro hilo a cpu-->replanificar
            enviar_respuesta_syscall_a_cpu(REPLANIFICACION);
            list_destroy_and_destroy_elements(params_io,free);
  
            break;
        case MUTEX_BLOQUEAR: // recurso.CPU me devuleve el control-> debo mandar algo a ejecutar
            t_list *params_lock = recibir_paquete(fd_conexion_cpu);
            char *recurso = list_get(params_lock, 0);

            sem_wait(&(semaforos->mutex_lista_exec));
            t_tcb *th_lock = list_get(lista_exec, 0);
            sem_post(&(semaforos->mutex_lista_exec));
            
            log_info(logger_kernel, "## (<%d>:<%d>) - Solicitó syscall: MUTEX_LOCK: %s",th_lock->pid, th_lock->tid,recurso);

            mutex_lock(recurso);
            list_destroy_and_destroy_elements(params_lock,free);

            break;
        case MUTEX_DESBLOQUEAR://recurso. CPU me devuleve el control-> debo mandar algo a ejecutar
            t_list *params_unlock = recibir_paquete(fd_conexion_cpu);
            char *recurso_unlok = (char*)list_get(params_unlock, 2);

            sem_wait(&(semaforos->mutex_lista_exec));
            t_tcb *th_unlock = list_get(lista_exec, 0);
            sem_post(&(semaforos->mutex_lista_exec));

            log_info(logger_kernel, "## (<%d>:<%d>) - Solicitó syscall: MUTEX_UNLOCK: %s", th_unlock->pid, th_unlock->tid, recurso_unlok);

            mutex_unlock(recurso_unlok,th_unlock);
            list_destroy_and_destroy_elements(params_unlock,free);
        break;
        case HILO_JUNTAR://tid_target. CPU me devuleve el control-> debo mandar algo a ejecutar
            t_list *params_juntar = recibir_paquete(fd_conexion_cpu);
            int tid_target = *((int *)list_get(params_juntar, 0));

            sem_wait(&(semaforos->mutex_lista_exec));
            t_tcb *th_en_exec = (t_tcb *)list_get(lista_exec, 0);
            sem_post(&(semaforos->mutex_lista_exec));

            log_info(logger_kernel, "## (<%d>:<%d>) - Solicitó syscall: HILO_JUNTAR a TID: %d",th_en_exec->pid,th_en_exec->tid, tid_target);
            
            thread_join(th_en_exec,tid_target);
            list_destroy_and_destroy_elements(params_juntar,free);
        break;

        case HILO_SALIR:
            sem_post (&(semaforos->sem_finalizacion_ejecucion_cpu));
            t_list *elpaquete_salir = recibir_paquete(fd_conexion_cpu);// recibo el paquete para no tener basura en el socket
            
            // quito de exec
            sem_wait(&(semaforos->mutex_lista_exec));
            t_tcb *thread_saliente = (t_tcb *)list_get(lista_exec, 0);
            sem_post(&(semaforos->mutex_lista_exec));

            log_info(logger_kernel, "## (<%d>:<%d>) - Solicitó syscall: FINALIZAR_HILO",thread_saliente->pid,thread_saliente->tid);

            thread_exit(thread_saliente);

            ///hacemos unicamente la eliminacion de la list
            sem_wait(&(semaforos->mutex_lista_exec));
            t_tcb *tid_en_exec = list_remove(lista_exec, 0);
            sem_post(&(semaforos->mutex_lista_exec));

            log_trace(logger_kernel, "Hilo con PID:%d y TID:%d salio de EXEC, es decir, finalizo :) ", tid_en_exec->pid, tid_en_exec->tid);
            //pasar_execute_a_exit(); este pasaje puede estar de mas, en thread exit ya se libera la memoria

        //  log_trace(logger_kernel, "HILO EN EXEC luego de desalojar...");
        //  mostrar_tcbs(lista_exec,logger_kernel);
            
            log_warning(logger_kernel, "HILOS PENDIENTES EN READY");
            mostrar_tcbs(lista_ready,logger_kernel);
            log_warning(logger_kernel, "HILOS PENDIENTES EN BLOCKED");
            mostrar_tcbs(lista_blocked,logger_kernel);

            // marca la cpu como libre
            enviar_respuesta_syscall_a_cpu(REPLANIFICACION); 
            //RTA A REPLANIFICACION        ACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA               
            sem_post(&(semaforos->espacio_en_cpu));
            list_destroy_and_destroy_elements(elpaquete_salir,free);  //este es nuevo
            
            break;
        case HILO_CANCELAR:
            t_list *params_th_cancel = recibir_paquete(fd_conexion_cpu);
            int tid = *((int *)list_get(params_th_cancel, 0));

            sem_wait(&(semaforos->mutex_lista_exec));
            t_tcb *thread_asociado = (t_tcb *)list_get(lista_exec, 0);
            sem_post(&(semaforos->mutex_lista_exec));

            log_info(logger_kernel, "## (<%d>:<%d>) - Solicitó syscall: CANCELAR_HILO con TID: %d",thread_asociado->pid,thread_asociado->tid, tid);

            thread_cancel(tid,thread_asociado->pid);
            //enviar_thread_a_cpu(thread_asociado,fd_conexion_cpu);
            enviar_respuesta_syscall_a_cpu(CONTINUA_EJECUTANDO_HILO);
            list_destroy_and_destroy_elements(params_th_cancel,free);
            
            break;
        case PEDIDO_MEMORY_DUMP:
            t_list* dump_args=recibir_paquete(fd_conexion_cpu);

            sem_wait(&(semaforos->mutex_lista_exec));
            t_tcb *thread_dump = (t_tcb *)list_get(lista_exec, 0);
            sem_post(&(semaforos->mutex_lista_exec));

            log_info(logger_kernel, "## (<%d>:<%d>) - Solicitó syscall: MEMORY_DUMP",thread_dump->pid,thread_dump->tid);

            sem_post (&(semaforos->sem_finalizacion_ejecucion_cpu));
            memory_dump();
            sem_wait(&(semaforos->conexion_memoria_dump));
            // marca la cpu como libre
            //printf("Mande el DUMP Y SIGO\n");
            enviar_respuesta_syscall_a_cpu(REPLANIFICACION);
            sem_post(&(semaforos->espacio_en_cpu));
            list_destroy_and_destroy_elements(dump_args,free);
            break;
        case FIN_DE_QUANTUM://Interrupcion
            t_list *params_fin_q = recibir_paquete(fd_conexion_cpu);
            int pid_desalojo=*((int *)list_get(params_fin_q, 0));
            int tid_desalojo=*((int *)list_get(params_fin_q, 1));  

            log_trace(logger_kernel, "## (<%d>:<%d>) FIN_DE_QUANTUM_OK",pid_desalojo,tid_desalojo);
                    
            manejar_interrupcion_fin_quantum();
            list_destroy_and_destroy_elements(params_fin_q,free);
        break;
        default:
            log_trace(logger_kernel," OPERACION INVALIDA RECIBIDA DE CPU ");
            
            return EXIT_FAILURE;
         break;
        }
    }
}

void manejar_interrupcion_fin_quantum(){
    // Obtener el TCB del hilo que se ejecutó durante el quantum
    pasar_execute_a_ready();
    //habilito planificador y marco cpu como libre
    sem_post(&(semaforos->contador_threads_en_ready));
    sem_post(&(semaforos->espacio_en_cpu));
}

void mostrar_pcb(t_pcb *pcb, t_log *logger)
{
    // Verificamos que el PCB no sea NULL
    if (pcb == NULL)
    {
        log_error(logger, "El PCB es NULL");
        return;
    }

    log_info(logger, "Mostrando información del PCB:");
    log_info(logger, "PID: %d", pcb->pid);
    log_info(logger, "Contador auto-incremental de TIDs: %d", pcb->contador_AI_tids);

    // Mostrar lista de TIDs
    if (pcb->lista_tids != NULL)
    {
        log_info(logger, "Lista de TIDs:");
        for (int i = 0; i < list_size(pcb->lista_tids); i++)
        {
            int tid = *(int *)list_get(pcb->lista_tids, i);
            log_info(logger, "\tTID #%d: %d", i, tid);
        }
    }
    else
    {
        log_warning(logger, "La lista de TIDs es NULL");
    }

    // Mostrar lista de Mutex
    if (pcb->lista_mutex != NULL)
    {
        log_info(logger, "Lista de Mutex:");
        for (int i = 0; i < list_size(pcb->lista_mutex); i++)
        {
            t_mutex *mutex_aux = list_get(pcb->lista_mutex, i);
            log_info(logger, "\tMutex #%d: %s", i, mutex_aux->recurso);
        }
    }
    else
    {
        log_warning(logger, "La lista de Mutex es NULL");
    }

    log_info(logger, "Tamaño del proceso: %d", pcb->tamanio_proceso);

    if (pcb->ruta_pseudocodigo != NULL)
    {
        log_info(logger, "Ruta del pseudocódigo: %s", pcb->ruta_pseudocodigo);
    }
    else
    {
        log_warning(logger, "La ruta del pseudocódigo es NULL");
    }
}

void mostrar_tcbs(t_list* lista_tcb, t_log* logger) {
    if (lista_tcb == NULL || list_size(lista_tcb) == 0) {
        log_info(logger, "No hay hilos en la lista.");
        return;
    }

    log_info(logger, "Lista de TCBs:");
    for (int i = 0; i < list_size(lista_tcb); i++) {
        t_tcb* tcb = list_get(lista_tcb, i);

        log_info(logger, "TCB #%d:", i + 1);
        log_info(logger, "\tTID: %d", tcb->tid);
        log_info(logger, "\tPrioridad: %d", tcb->prioridad);
        log_info(logger, "\tPID: %d", tcb->pid);
        log_info(logger, "\tTiempo de IO: %d", tcb->tiempo_de_io);
        //log_info(logger, "\tEstado: %d", tcb->estado);  // Usa una función para convertir el estado a texto si es necesario

        if (tcb->thread_target != NULL) {
            t_tcb* target = (t_tcb*) tcb->thread_target;
            log_info(logger, "\tEsperando hilo TID: %d", target->tid);
        } else {
            log_info(logger, "\tNo está esperando ningún hilo.");
        }
        
        // Mostrar los mutex asignados
        if (tcb->mutex_asignados != NULL && list_size(tcb->mutex_asignados) > 0) {
            log_info(logger, "\tMutex asignados:");
            for (int j = 0; j < list_size(tcb->mutex_asignados); j++) {
                t_mutex* mutex = list_get(tcb->mutex_asignados, j);
                log_info(logger, "\t\tRecurso: %s", mutex->recurso);
            }
        } else {
            log_info(logger, "\tNo tiene mutex asignados.");
        }
    }
}