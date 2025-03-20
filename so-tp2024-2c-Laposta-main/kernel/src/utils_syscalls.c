#include <../include/init_kernel.h>


t_pcb* crear_pcb(int tam_proceso,char* archivo_instrucciones,int prioridad_th0) {
    t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->contador_AI_tids=0;//inicializa el contador de tids del proceso
    pcb->lista_mutex=list_create();
    pcb->lista_tids=list_create();
    pcb->tamanio_proceso=tam_proceso;
    pcb->pid=pid_AI_global;
    pid_AI_global++;
    pcb->prioridad_th_main=prioridad_th0;
    pcb->ruta_pseudocodigo=strdup(archivo_instrucciones);
 
    return pcb;
}
void anadir_tid_a_proceso(t_pcb* pcb){
    list_add(pcb->lista_tids,&(pcb->contador_AI_tids));
    pcb->contador_AI_tids++;
}

void enviar_solicitud_espacio_a_memoria(t_pcb* pcb,int socket){
    //memoria solo necesita tamanio de proceso y el pid 
    t_paquete* paquete_a_enviar=crear_paquete(INICIAR_PROCESO);
    agregar_a_paquete(paquete_a_enviar,&(pcb->pid),sizeof(uint32_t));

    agregar_a_paquete(paquete_a_enviar,&(pcb->tamanio_proceso),sizeof(uint32_t));
    enviar_paquete(paquete_a_enviar,socket);
    log_warning(logger_kernel,"envio a memoria crear proceso:%d, tamanio:%d",pcb->pid,pcb->tamanio_proceso);
    eliminar_paquete(paquete_a_enviar);
}
int recibir_resp_de_memoria_a_solicitud(int socket_memoria){
    int cod=-1;
    cod=recibir_operacion(socket_memoria);
    recibir_paquete(socket_memoria);
    log_trace(logger_kernel,"recibi paquete de memoria");
    //list_destroy_and_destroy_elements(paquete_respuesta_memoria,free);
    return cod;

}

//int asignar_tid(t_pcb* pcb)
//    int tid_a_asignar=pcb->contador_AI_tids;
//    list_add(pcb->lista_tids,&(tid_a_asignar));
//    pcb->contador_AI_tids++;
//    return tid_a_asignar;
//}
int asignar_tid(t_pcb* pcb) {
    int* tid_a_asignar = malloc(sizeof(int));  // Asignar memoria dinámica para el nuevo TID
    *tid_a_asignar = pcb->contador_AI_tids;    // Asignar el valor del TID
    list_add(pcb->lista_tids, tid_a_asignar);  // Agregar el TID a la lista
    pcb->contador_AI_tids++;                   // Incrementar el contador de TIDs
    return *tid_a_asignar;                     // Retornar el valor del nuevo TID
}

t_tcb* crear_tcb(int prioridad_th,int pid){
    t_tcb* nuevo_tcb=malloc(sizeof(t_tcb));
    nuevo_tcb->prioridad=prioridad_th;
    nuevo_tcb->tiempo_de_io=0;
    nuevo_tcb->mutex_asignados=list_create();
    nuevo_tcb->thread_target=NULL;
    t_pcb* pcb=buscar_proceso_por(pid);
    nuevo_tcb->tid=asignar_tid(pcb);
    nuevo_tcb->pid=pid;
    return nuevo_tcb;
}
void enviar_a_memoria_creacion_thread(t_tcb* tcb_nuevo,char* pseudo,int socket){
    t_paquete* paquete_a_enviar=crear_paquete(INICIAR_HILO);
    int longitud=strlen(pseudo)+1;
    agregar_a_paquete(paquete_a_enviar,&(tcb_nuevo->pid),sizeof(int));
    agregar_a_paquete(paquete_a_enviar,&(tcb_nuevo->tid),sizeof(int));
   agregar_a_paquete(paquete_a_enviar,pseudo,longitud);
 
    enviar_paquete(paquete_a_enviar,socket);
    log_warning(logger_kernel,"envio a memoria crear hilo pid:%d, tid:%d ,archivo:%s",tcb_nuevo->pid,tcb_nuevo->tid,pseudo);

    eliminar_paquete(paquete_a_enviar);  
}
//TODO: revisar semaforos
t_pcb* buscar_proceso_por(int pid_buscado){
    t_pcb* un_pcb=NULL;
    
    for(int i=0;i<list_size(lista_procesos_global);i++){
        un_pcb=(t_pcb*)list_get(lista_procesos_global,i);
        if(un_pcb->pid==pid_buscado){
            return un_pcb;
        }
    }
   
    return un_pcb;
}
void enviar_thread_a_cpu(t_tcb* tcb_a_ejetucar,int socket_dispatch){
    t_paquete * paquete=crear_paquete(PROCESO_EJECUTAR);
   
    uint32_t valor_pid=(uint32_t)(tcb_a_ejetucar->pid);
    uint32_t valor_tid=(uint32_t)(tcb_a_ejetucar->tid);
    agregar_a_paquete(paquete,&valor_pid,sizeof(uint32_t));
    agregar_a_paquete(paquete,&valor_tid,sizeof(uint32_t));
    sem_wait(&(semaforos->mutex_conexion_dispatch));
    enviar_paquete(paquete,socket_dispatch);
    sem_post(&(semaforos->mutex_conexion_dispatch));
    log_warning(logger_kernel,"envio hilo a cpu pid:%d, tid:%d ,socket:%d",tcb_a_ejetucar->pid,tcb_a_ejetucar->tid,socket_dispatch);
    eliminar_paquete(paquete);
}


//TODO: revisar semaforos
//busca el proceso en la lista global de procesos y dentro del proceso el mutex 
t_mutex* buscar_mutex(char* recurso,int pid){
    t_mutex *mutex=NULL;
    sem_wait(&(semaforos->mutex_lista_global_procesos));
    t_pcb *pcb=buscar_proceso_por(pid);
    sem_post(&(semaforos->mutex_lista_global_procesos));
    for (int i = 0; i < list_size(pcb->lista_mutex ); i++)
    {
        t_mutex* mutex_aux=(t_mutex*)list_get(pcb->lista_mutex,i);
        if(strcmp(recurso,mutex_aux->recurso)==0){
            mutex=(t_mutex*)list_get(pcb->lista_mutex,i);
            break;
        }
        
    }
    return mutex;
    
}
void asignar_mutex(t_tcb * tcb, t_mutex* mutex){
        mutex->estado=ASIGNADO;
        mutex->thread_asignado=tcb;
        list_add(tcb->mutex_asignados,mutex);
}


void* enviar_a_memoria_thread_saliente(void* t){
    t_tcb* tcb=(t_tcb*)t;
    t_paquete *paquete=crear_paquete(FINALIZAR_HILO);
    agregar_a_paquete(paquete,&(tcb->pid),sizeof(int));
    agregar_a_paquete(paquete,&(tcb->tid),sizeof(int));
    int fd_memoria=conectar_a_memoria();
    enviar_paquete(paquete,fd_memoria);
   
    
    //espero respuesta de memoria;

    int rta=recibir_resp_de_memoria_a_solicitud(fd_memoria);
    if(rta==FINALIZAR_HILO_RTA_OK){
        log_info(logger_kernel, "## (<%d>:<%d>) Finalizo el hilo",tcb->pid,tcb->tid);
        sem_post(&(semaforos->sem_espacio_liberado_por_proceso)); //EStO ACA NO
    }
    else{
        log_trace(logger_kernel, "(<%d>:<%d>) MEMORIA no logro Finalizar el hilo",tcb->pid,tcb->tid);
    }
    close(fd_memoria); 
    eliminar_paquete(paquete);
}

void* enviar_a_memoria_proceso_saliente(void* t){
    t_tcb* tcb=(t_tcb*)t;
    t_paquete *paquete=crear_paquete(FINALIZAR_PROCESO);
    agregar_a_paquete(paquete,&(tcb->pid),sizeof(uint32_t));
    int fd_memoria=conectar_a_memoria();
    enviar_paquete(paquete,fd_memoria);
   
    
    //espero respuesta de memoria;

    int rta=recibir_resp_de_memoria_a_solicitud(fd_memoria);
    if(rta==FINALIZAR_PROCESO_RTA_OK){
        log_info(logger_kernel, "## Finaliza el proceso %d",tcb->pid);
        sem_post(&(semaforos->sem_espacio_liberado_por_proceso)); //EStO ACA NO
    }
    else{
        log_trace(logger_kernel, "## MEMORIA no logro Finalizar el proceso %d",tcb->pid);
    }
    close(fd_memoria); 
    eliminar_paquete(paquete);
}

void destruir_tcb(t_tcb* tcb){
    if (tcb == NULL) {
        return; 
    }
    // Liberar la memoria reservada por el TCB
    free(tcb);
}
//TODO: uso semaforos??

//devuelve el indice del tid si existe o -1 si no
int buscar_indice_de_tid_en_proceso(t_pcb *pcb,int tid){
    int posicion=-1;

   for(int i=0;i<list_size(pcb->lista_tids);i++){
        bool pcb_tiene_tid=*((int *)list_get(pcb->lista_tids,i))==tid;
        if(pcb_tiene_tid){
            posicion=i;
            return posicion;
        }
    }
    return posicion;
}
//TODO: uso semaforos??
//elimina el tid(si existe) del pcb
 bool quitar_tid_de_proceso(t_tcb* tcb_saliente){
    bool exito_eliminando_de_pcb=false;
    t_pcb *pcb=buscar_proceso_por(tcb_saliente->pid);
    int posicion_a_eliminar=buscar_indice_de_tid_en_proceso(pcb,tcb_saliente->tid);
    if(posicion_a_eliminar!=-1){
        list_remove(pcb->lista_tids,posicion_a_eliminar);
        log_info("## (<%d>:<%d>) Finaliza el hilo",tcb_saliente->pid,tcb_saliente->tid);
        exito_eliminando_de_pcb=true;
    }
        
    return exito_eliminando_de_pcb;
 }
// REVISAR: agregar la funcion utilizada en thread_cancel para ir hilo por hilo mandando a memoria y que finalice el hilo en cuestion. Ya esta
 //Paso un pcb y cancelo todos los tcb asociados a ese pcb en la lista en cuestion
void buscar_y_cancelar_tcb_asociado_a_pcb(int pid,t_list* lista_en_custion,sem_t* sem,t_estado lista_estado){
    sem_wait(sem);
    t_tcb* tcb=NULL;
    for(int i=0;i<list_size(lista_en_custion);i){
        t_tcb* tcb_aux=(t_tcb*)list_get(lista_en_custion,i);
        if(tcb_aux->pid==pid){
            tcb=tcb_aux;
            pthread_t hilo_manejo_exit;
            pthread_create(&hilo_manejo_exit,NULL,enviar_a_memoria_thread_saliente,(void*)tcb);
            pthread_detach(hilo_manejo_exit);
            list_remove(lista_en_custion,i);
            if(lista_estado==READY){
                sem_wait(&(semaforos->contador_threads_en_ready));
            }
        }else{
            i++;
        }
    }
    sem_post(sem);
}

//busca un tcb en una lista por su tid y pid y lo remueve
t_tcb* buscar_en_lista_y_cancelar(t_list* lista,int tid,int pid,sem_t* sem){
    sem_wait(sem);
    log_trace(logger_kernel,"Entre en buscar lista y cancelar para pid: %d tid :%d ",pid, tid );
    for(int i=0;i<list_size(lista);i++){
        t_tcb* tcb=(t_tcb*)list_get(lista,i);
        if(tcb->tid==tid && tcb->pid==pid){
            list_remove(lista,i);
            sem_post(sem);
            return tcb;
        }
    }
    sem_post(sem);
    return NULL;
}
t_tcb* buscar_en_lista_tcb(t_list* lista,int tid,int pid,sem_t* sem){
    sem_wait(sem);
    for(int i=0;i<list_size(lista);i++){
        t_tcb* tcb=(t_tcb*)list_get(lista,i);
        if(tcb->tid==tid && tcb->pid==pid){
            sem_post(sem);
            return tcb;
        }
    }
    sem_post(sem);
    return NULL;
}
t_mutex* quitar_mutex_a_thread(char* recurso,t_tcb* tcb){
    t_mutex *mutex=NULL;
    for(int i=0;i<list_size(tcb->mutex_asignados);i++){
         mutex=(t_mutex*)list_get(tcb->mutex_asignados,i);
        if(strcmp(recurso,mutex->recurso)==0){
            list_remove(tcb->mutex_asignados,i);
            return mutex;
        }
    }
    return mutex;
}
t_tcb* asignar_mutex_al_siguiente_thread(t_mutex* mutex){
    t_tcb* tcb=NULL;
    if(list_size(mutex->lista_threads_bloquedos)>0){
    
    tcb=(t_tcb*)list_remove(mutex->lista_threads_bloquedos,0);
        asignar_mutex(tcb,mutex);
        mutex->thread_asignado=tcb;
    }
    return tcb;
}
void inicializar_hilo_intefaz_io(){
    pthread_t interfaz_entrada_salida;
    pthread_create(&interfaz_entrada_salida,NULL,interfaz_io,NULL);
    pthread_detach(interfaz_entrada_salida);

    pthread_t h_hilo_sleep_io;
    pthread_create(&h_hilo_sleep_io,NULL,hilo_sleep_io,NULL);
    pthread_detach(h_hilo_sleep_io);
}

void interfaz_io(){
    while(1){
        sem_wait (&(semaforos->sem_io_solicitud));

        sem_wait(&(semaforos->mutex_lista_espera_io));
        t_tcb* tcb_io = list_get (lista_espera_io,0);
        sem_post(&(semaforos->mutex_lista_espera_io));

        log_trace(logger_kernel,"TCB OBTENIDO de la lista de espera de io: %d", tcb_io->tid);

        sem_post(&(semaforos->sem_sleep_io));

        sem_wait(&(semaforos->sem_io_sleep_en_uso)); 
          sem_wait(&(semaforos->mutex_lista_espera_io));
        t_tcb* tcb_usando_io = list_remove (lista_espera_io,0);
          sem_post(&(semaforos->mutex_lista_espera_io));
        agregar_a_lista(tcb_usando_io,lista_ready,&(semaforos->mutex_lista_ready));
        buscar_en_lista_y_cancelar(lista_blocked,tcb_usando_io->tid,tcb_usando_io->pid,&(semaforos->mutex_lista_blocked));
        sem_post(&(semaforos->contador_threads_en_ready));
    }
}

void hilo_sleep_io() {
    while (1) {
        sem_wait(&(semaforos->sem_sleep_io));
        
        sem_wait(&(semaforos->mutex_lista_espera_io));
        if (list_size(lista_espera_io) > 0) {
            t_tcb* tcb_usando_io = list_get(lista_espera_io, 0);
            sem_post(&(semaforos->mutex_lista_espera_io));

            int tiempo = tcb_usando_io->tiempo_de_io / 1000;
            log_info(logger_kernel, "## IO en uso por %d milisegundos", tiempo);
            sleep(tiempo);
            log_info(logger_kernel,"## (<%d><%d>) Finalizó IO y pasa a READY", tcb_usando_io->pid, tcb_usando_io->tid);
            sem_post(&(semaforos->sem_io_sleep_en_uso));
        } else {
            sem_post(&(semaforos->mutex_lista_espera_io));
            log_error(logger_kernel, "Lista de espera IO vacía en hilo_sleep_io.");
        }
    }
}



//TODO: FIXME: controlar semaforos
//recorre las lista de bloqueados y desbloquea tcb por mutex o join 
void desbloquear_hilos_por_fin_de_hilo(t_tcb* tcb_finalizado){
    log_warning(logger_kernel,"## Desbloqueo de hilos por fin de hilo. Tid finalizando: %d",tcb_finalizado->tid);
    sem_wait(&(semaforos->mutex_lista_blocked));
    mostrar_tcbs(lista_blocked,logger_kernel);
    for (int i = 0 ; i < list_size(lista_blocked); i++) {
        t_tcb* tcb_bloqueado = list_get(lista_blocked, i);
        log_warning(logger_kernel,"entre en el for %d vez .tamanio lista block:%d",i,list_size(lista_blocked));
        //desbloquea por join
        if (tcb_bloqueado->thread_target != NULL &&
            ((t_tcb*)(tcb_bloqueado->thread_target))->pid == tcb_finalizado->pid &&
            ((t_tcb*)(tcb_bloqueado->thread_target))->tid == tcb_finalizado->tid  ) {
            
            list_remove(lista_blocked, i);
            i--; // Ajustar el índice después de eliminar
            log_warning(logger_kernel,"desbloqueando hilo pid:%d, tid:%d ",tcb_bloqueado->pid,tcb_bloqueado->tid);
            tcb_bloqueado->thread_target=NULL;
            agregar_a_lista(tcb_bloqueado,lista_ready,&(semaforos->mutex_lista_ready));

            sem_post(&(semaforos->contador_threads_en_ready));
            continue;
        }
        bool desbloqueado=false;
        //recorre todos los mutex del que finaliza
        for (int j= 0; j < list_size(tcb_finalizado->mutex_asignados)&& !desbloqueado; j++){
            t_mutex* mute_aux=list_get(tcb_finalizado->mutex_asignados,j);
           
            //recorre los tcb bloq de cada mutex
            for (int k = 0; k < list_size(mute_aux->lista_threads_bloquedos) ; k++)
            {
                t_tcb* tcb_aux=(t_tcb*)list_get(mute_aux->lista_threads_bloquedos,k);
                //desbloquea por mutex
                if(tcb_aux->tid==tcb_bloqueado->tid){
                    list_remove(lista_blocked,i);
                    i--; // Ajustar el índice después de eliminar
            log_warning(logger_kernel,"desbloqueando hilo pid:%d, tid:%d ",tcb_bloqueado->pid,tcb_bloqueado->tid);

                    list_remove(mute_aux->lista_threads_bloquedos,k);
                    mute_aux->thread_asignado=NULL;
                    mute_aux->estado=SIN_ASIGNAR;
                    agregar_a_lista(tcb_aux,lista_ready,&(semaforos->mutex_lista_ready));
                    sem_post(&(semaforos->contador_threads_en_ready));
                    desbloqueado=true;
                    break;
                }
            }  
        }  

    }
    sem_post(&(semaforos->mutex_lista_blocked));
   
}
