#include "../include/init_kernel.h"

//No se si esto esta bien aca
t_list* lista_new;
t_list* lista_ready;
t_list* lista_exec;
t_list* lista_blocked;
t_list* lista_exit;
t_list* lista_procesos_global;
t_list* lista_espera_io;
t_list* lista_mutex;

void inicializar_listas() {
    lista_new = list_create();
    lista_ready = list_create();
    lista_exec = list_create();
    lista_exit = list_create();
    lista_blocked = list_create();
    lista_procesos_global=list_create();
    lista_espera_io = list_create();
    lista_mutex = list_create();
}
void inicializar_hilos_largo_plazo(){
    pthread_create(&(hilos->hilo_planif_largo_plazo),NULL,planificar_procesos,NULL);
    pthread_create(&(hilos->hilo_finalizacion_procesos_memoria),NULL,manejo_liberacion_memoria,NULL);

}

void* planificar_procesos(){
    
    while (1) {

        sem_wait(&(semaforos->sem_procesos_new));//se ingreso un proceso
      

        sem_wait(&(semaforos->mutex_lista_new));
        t_pcb* un_pcb=NULL;
        un_pcb=list_get(lista_new,0);
        sem_post(&(semaforos->mutex_lista_new));
                    // chequeamos si el pcb no es null


        if (un_pcb == NULL) {
            
            continue;
        }else{
            int socket_memoria=conectar_a_memoria();
            enviar_solicitud_espacio_a_memoria(un_pcb,socket_memoria);
          
            int respuesta=recibir_resp_de_memoria_a_solicitud(socket_memoria);
            close(socket_memoria);
            if(respuesta==INICIAR_PROCESO_RTA_OK){
                log_trace(logger_kernel,"recibi ok para crear proceso");
               t_tcb* tcb=thread_create(un_pcb->ruta_pseudocodigo,un_pcb->prioridad_th_main ,un_pcb->pid);//creo el thread main y lo envio a ready 

                // pasar_new_a_ready();  TODO: no se puede usar por que en NEW hay procesos y en READY hay hilos
                //FIXME: REMUEVO el pcb de new por fifo, el pcb aun esta en lista_global_procesos
                remover_de_lista(lista_new,0,&(semaforos->mutex_lista_new));
                agregar_a_lista(tcb,lista_ready,&(semaforos->mutex_lista_ready));
                log_trace(logger_kernel, "nuevo proceso con pid %d y tid %d",tcb->pid,tcb->tid);
                t_tcb* prueba_tcb=(t_tcb*)list_get(lista_ready,0);

                //Le avisamos a planif_corto_plazo que tiene un thread en ready
                sem_post(&(semaforos->contador_threads_en_ready));
            }else{
                log_trace(logger_kernel, "No hay espacio en memoria para proc PCB:%d, se esperara liberacion por parte de otro proceso \n", un_pcb->pid);
                log_trace(logger_kernel, "Codigo de op recibido: %d", respuesta);
                //el proceso continua en new hasta que se elimine otro proceso(EXIT)
            }
                 
        }
    }
    return NULL; 
}


void manejo_liberacion_memoria(){
    while(1){
            sem_wait(&(semaforos->sem_espacio_liberado_por_proceso));
        if(list_is_empty(lista_new)){
            log_trace(logger_kernel,"No hay procesos en memoria para liberar \n");
        }else{
            log_trace(logger_kernel,"Se libero espacio en memoria");
            sem_post(&(semaforos->sem_procesos_new));
        }
    }
}


void mover_procesos(t_list* lista_origen, t_list* lista_destino, sem_t* sem_origen, sem_t* sem_destino, t_estado nuevo_estado) {
    if (!list_is_empty(lista_origen)) {
        sem_wait(sem_origen);
        t_tcb* tcb = (t_tcb*)list_remove(lista_origen, 0);
        sem_post(sem_origen);
        if (nuevo_estado == EXIT){
            log_info(logger_kernel, "Hilo con TID %d y PID %d movido a la lista EXIT", tcb->tid, tcb->pid);
            //aca deberia mandar a memoria la eliminacion del hilo
            pthread_t hilo_manejo_exit;
            pthread_create(&hilo_manejo_exit,NULL,enviar_a_memoria_proceso_saliente,(void*)tcb);
            pthread_detach(hilo_manejo_exit);
            
        }else{
            sem_wait(sem_destino);
            tcb->estado = nuevo_estado;
            list_add(lista_destino, tcb);
            sem_post(sem_destino);

        if (nuevo_estado == NEW) {
            log_info(logger_kernel, "Hilo con TID %d y PID %d movido a la lista NEW", tcb->tid, tcb->pid);
        }
        else if(nuevo_estado == READY){
            log_info(logger_kernel, "Hilo con TID %d y PID %d movido a la lista READY", tcb->tid, tcb->pid);
        }
        else if(nuevo_estado == EXEC){
            log_info(logger_kernel, "Hilo con TID %d y PID %d movido a la lista EXEC", tcb->tid, tcb->pid);
        }
        else if(nuevo_estado == BLOCKED){
            log_info(logger_kernel, "Hilo con TID %d y PID %d movido a la lista BLOCKED", tcb->tid, tcb->pid);
        }else 
            log_info(logger_kernel, "No hay hilos en la lista origen");
        }
        }
        
}


void agregar_a_cola(t_pcb *pcb,t_list* lista,sem_t* sem){
    sem_wait(sem);
    list_add(lista,pcb);
    sem_post(sem);
}
void pasar_new_a_ready() {
    mover_procesos(lista_new, lista_ready, &(semaforos->mutex_lista_new), &(semaforos->mutex_lista_ready), READY); 
}
void pasar_ready_a_exit() { //se usara? ver finalizar proceso
    mover_procesos(lista_ready, lista_exit, &(semaforos->mutex_lista_ready), &(semaforos->mutex_lista_exit), EXIT);
}
void pasar_new_a_exit() { //se usara? ver finalizar proceso
    mover_procesos(lista_new, lista_exit, &(semaforos->mutex_lista_new), &(semaforos->mutex_lista_exit), EXIT);
}
void agregar_a_lista(t_tcb *tcb,t_list* lista,sem_t* sem){
    sem_wait(sem);
    list_add(lista,tcb);
    sem_post(sem);
}
t_tcb* remover_de_lista(t_list* lista,int indice, sem_t* mutex){
    t_tcb* tcb=NULL;
    sem_wait(mutex);
    tcb=(t_tcb*)list_remove(lista,indice);
    sem_post(mutex);
    return tcb;
}

