#include "../include/memoria_usuario.h"




void inicializar_proceso(uint32_t pid, uint32_t tamanio){
    t_miniPCB* nuevo_proceso = malloc(sizeof(t_miniPCB));
    //t_hilo* nuevo_hilo = malloc(sizeof(t_hilo));

    nuevo_proceso->pid = pid;
    nuevo_proceso->hilos = list_create();

 
    uint32_t indice_bloque_a_liberar = buscar_indice_bloque_por_pid(pids_por_bloque,pid);

    log_trace(logger_memoria,"El indice en la lista del bloque a liberar es: %d\n",indice_bloque_a_liberar);

    t_pid_por_bloque* bloque_x_pid = list_get(pids_por_bloque,indice_bloque_a_liberar);

    nuevo_proceso->base = calcular_base_proceso_fijas(bloque_x_pid->bloque, lista_particiones);

    nuevo_proceso->limite = nuevo_proceso->base + tamanio;

    //list_add(nuevo_proceso->hilos,nuevo_hilo);
    pthread_mutex_lock(&mutex_lista_miniPCBs);
    list_add(lista_miniPCBs,nuevo_proceso);
    pthread_mutex_unlock(&mutex_lista_miniPCBs);
}


//Crear un Proceso
int crear_proceso_fijas(uint32_t tam_proceso, t_list* lista_de_particiones, uint32_t pid) {
    log_trace(logger_memoria,"Entro crear proceso\n");
    log_trace(logger_memoria,"tam_proceso:%d,pid:%d,algoritmo_alocacion:%s\n",tam_proceso,pid,algoritmo_alocacion);
    log_trace(logger_memoria,"Tamanio lista de particiones:%d\n", list_size(lista_de_particiones));
    //encontrar hueco libre y marcar bitmap, si no encuentra tira error
    uint32_t tamanio_bloque_actual = 0;
    bool bloque_libre_encontrado = false;

    if(tam_proceso == 0){
        return -1;
    }
    else{
        if (strcmp(algoritmo_alocacion, "FIRST") == 0) {
            log_trace(logger_memoria,"Entro FIRST\n");

            for (int i = 0; i < list_size(lista_de_particiones); i++) {
                log_trace(logger_memoria,"Entro loop %d\n", i);

                // Obtiene el puntero a char* desde la lista
                char* tamanio_bloque_str = (char*)list_get(lista_de_particiones, i);

                // Verifica que el puntero no sea nulo
                if (tamanio_bloque_str == NULL) {
                    log_trace(logger_memoria,"Error: puntero a tamaño de bloque es NULL para índice %d\n", i);
                    continue; // Salta al siguiente elemento de la lista
                }

                // Convierte el char* a uint32_t
                tamanio_bloque_actual = (uint32_t)atoi(tamanio_bloque_str);
                log_trace(logger_memoria,"El tamanio del bloque %d es: %d\n", i, tamanio_bloque_actual);

                if (tam_proceso <= tamanio_bloque_actual && !bitarray_test_bit(bitmap_particiones,i)) {
                    bloque_libre_encontrado = true;
                    log_trace(logger_memoria,"Elijo bloque %d para proceso con PID %d de tamaño %d\n", i,pid,tam_proceso);

                    pthread_mutex_lock(&mutex_pids_por_bloque);
                    bitarray_set_bit(bitmap_particiones, i);
                    pthread_mutex_unlock(&mutex_pids_por_bloque);
                    t_pid_por_bloque* pid_por_bloque = malloc(sizeof(t_pid_por_bloque));
                    pid_por_bloque->pid = pid;
                    pid_por_bloque->bloque = i;
                    //PENDIENTE:Verificar que el pid no este ya en memoria
                    pthread_mutex_lock(&mutex_pids_por_bloque);
                    list_add(pids_por_bloque,pid_por_bloque);
                    pthread_mutex_unlock(&mutex_pids_por_bloque);
                    inicializar_proceso(pid, tamanio_bloque_actual); //VER UBICACION
                    return INICIAR_PROCESO_RTA_OK;
                }
            }

            if (!bloque_libre_encontrado) {
                log_trace(logger_memoria,"No encuentro bloque\n");
                return -1;
            }
        }
        else if(strcmp(algoritmo_alocacion, "BEST") == 0){
            uint32_t ultimo_bloque_best_fit = -1;
            uint32_t tamanio_ultimo_bloque_best_fit = 0;

            for (int i = 0; i < list_size(lista_de_particiones); i++){
                // Obtiene el puntero a char* desde la lista
                char* tamanio_bloque_str = (char*)list_get(lista_de_particiones, i);

                // Verifica que el puntero no sea nulo
                if (tamanio_bloque_str == NULL) {
                    log_error(logger_memoria,"Error: puntero a tamaño de bloque es NULL para índice %d\n", i);
                    continue; // Salta al siguiente elemento de la lista
                }

                // Convierte el char* a uint32_t
                tamanio_bloque_actual = (uint32_t)atoi(tamanio_bloque_str);
                log_trace(logger_memoria,"El tamanio del bloque %d es: %d\n", i, tamanio_bloque_actual);
                if(tam_proceso<=tamanio_bloque_actual && !bitarray_test_bit(bitmap_particiones,i)){ //El proceso entra en el bloque actual
                    if(tamanio_ultimo_bloque_best_fit == 0 || (tamanio_bloque_actual<tamanio_ultimo_bloque_best_fit)){
                        tamanio_ultimo_bloque_best_fit = tamanio_bloque_actual;
                        ultimo_bloque_best_fit = i;
                        log_trace(logger_memoria,"El ultimo_bloque_best_fit es: %d\n", ultimo_bloque_best_fit);
                        bloque_libre_encontrado = true;
                    }
                }
            }

            if(!bloque_libre_encontrado){
                return -1;
            }
            else {
                log_trace(logger_memoria,"Elijo bloque %d para proceso con PID %d de tamaño %d\n", ultimo_bloque_best_fit,pid,tam_proceso);
                pthread_mutex_lock(&mutex_bitmap_particiones);
                bitarray_set_bit(bitmap_particiones, ultimo_bloque_best_fit);
                pthread_mutex_unlock(&mutex_bitmap_particiones);
                 t_pid_por_bloque* pid_por_bloque = malloc(sizeof(t_pid_por_bloque));
                    pid_por_bloque->pid = pid;
                    pid_por_bloque->bloque = ultimo_bloque_best_fit;
                    //PENDIENTE:Verificar que el pid no este ya en memoria
                    pthread_mutex_lock(&mutex_pids_por_bloque);
                    list_add(pids_por_bloque,pid_por_bloque);
                    pthread_mutex_unlock(&mutex_pids_por_bloque);
                    log_trace(logger_memoria,"Se agrego a lista. Estado actual:\n");
                    print_lista_pid_por_bloque(pids_por_bloque);
                    inicializar_proceso(pid, tamanio_ultimo_bloque_best_fit); //VER UBICACION
                     return INICIAR_PROCESO_RTA_OK; 
            }
        }
        else if(strcmp(algoritmo_alocacion, "WORST") == 0){
            uint32_t ultimo_bloque_worst_fit = -1;
            uint32_t tamanio_ultimo_bloque_worst_fit = 0;

            for (int i = 0; i < list_size(lista_de_particiones); i++){
                // Obtiene el puntero a char* desde la lista
                char* tamanio_bloque_str = (char*)list_get(lista_de_particiones, i);

                // Verifica que el puntero no sea nulo
                if (tamanio_bloque_str == NULL) {
                    log_error(logger_memoria,"Error: puntero a tamaño de bloque es NULL para índice %d\n", i);
                    continue; // Salta al siguiente elemento de la lista
                   

                }

                // Convierte el char* a uint32_t
                tamanio_bloque_actual = (uint32_t)atoi(tamanio_bloque_str);
                log_trace(logger_memoria,"El tamanio del bloque %d es: %d\n", i, tamanio_bloque_actual);
                if(tam_proceso<=tamanio_bloque_actual && !bitarray_test_bit(bitmap_particiones,i)){ //El proceso entra en el bloque actual
                    if(tamanio_ultimo_bloque_worst_fit == 0 || (tamanio_bloque_actual>tamanio_ultimo_bloque_worst_fit)){
                        tamanio_ultimo_bloque_worst_fit = tamanio_bloque_actual;
                        ultimo_bloque_worst_fit = i;
                        bloque_libre_encontrado = true;
                    }
               
                }
            }

            if(!bloque_libre_encontrado){
                log_trace(logger_memoria,"salio por !bloque_libre_encontrado");
                return -1;
            }
            else {
                log_trace(logger_memoria,"Elijo bloque %d para proceso con PID %d de tamaño %d\n", ultimo_bloque_worst_fit,pid,tam_proceso);
                pthread_mutex_lock(&mutex_bitmap_particiones);
                bitarray_set_bit(bitmap_particiones, ultimo_bloque_worst_fit);
                pthread_mutex_unlock(&mutex_bitmap_particiones);
                t_pid_por_bloque* pid_por_bloque = malloc(sizeof(t_pid_por_bloque));
                    pid_por_bloque->pid = pid;
                    pid_por_bloque->bloque = ultimo_bloque_worst_fit;
                    //PENDIENTE:Verificar que el pid no este ya en memoria
                    pthread_mutex_lock(&mutex_pids_por_bloque);
                    list_add(pids_por_bloque,pid_por_bloque);
                    pthread_mutex_unlock(&mutex_pids_por_bloque);
                    inicializar_proceso(pid, tamanio_ultimo_bloque_worst_fit); //VER UBICACION
                
                return INICIAR_PROCESO_RTA_OK;
            }
        }
        else{
            log_error(logger_memoria,"Error: algoritmo incorrecto\n");
            return -1;
        }
            log_trace(logger_memoria,"salio por return 0");
        return 0;
    }
}

void finalizar_proceso_fijas(uint32_t pid){
    uint32_t indice_bloque_a_liberar = buscar_indice_bloque_por_pid(pids_por_bloque,pid);

    log_trace(logger_memoria,"El indice en la lista del bloque a liberar es: %d\n",indice_bloque_a_liberar);

    t_pid_por_bloque* bloque_x_pid = list_get(pids_por_bloque,indice_bloque_a_liberar);
    log_trace(logger_memoria,"bloque_x_pid: %d\n",bloque_x_pid->bloque);
    pthread_mutex_lock(&mutex_bitmap_particiones);
    bitarray_clean_bit(bitmap_particiones,bloque_x_pid->bloque);
    pthread_mutex_unlock(&mutex_bitmap_particiones);
    pthread_mutex_lock(&mutex_pids_por_bloque);
    list_remove_and_destroy_element(pids_por_bloque,indice_bloque_a_liberar,free);
    pthread_mutex_unlock(&mutex_pids_por_bloque);

   eliminar_proceso_de_lista(pid);
    
}


void uint32_to_string(uint32_t num, char *str, size_t size) {
    snprintf(str, size, "%u", num);
}

uint32_t buscar_indice_bloque_por_pid(t_list* lista, uint32_t pid) {
    for (int i = 0; i < list_size(lista); i++) {
        t_pid_por_bloque* pid_por_bloque = list_get(lista, i);
        if (pid_por_bloque->pid == pid) {
            return i;
        }
    }
    return -1; 
}



// Función para imprimir un elemento de tipo t_pid_por_bloque
void print_pid_por_bloque(void* element) {
    t_pid_por_bloque* pid_por_bloque = (t_pid_por_bloque*)element;
    log_trace(logger_memoria,"PID: %u, Bloque: %u\n", pid_por_bloque->pid, pid_por_bloque->bloque);
}



// Función para imprimir una lista de t_pid_por_bloque
void print_lista_pid_por_bloque(t_list* lista) {
    log_trace(logger_memoria,"Contenido de la lista de PID por bloque:\n");

    // Verifica que la lista no sea NULL
    if (lista == NULL) {
        log_trace(logger_memoria,"La lista es NULL.\n");
        return;
    }

    // Recorre cada elemento de la lista e imprime su contenido
    for (int i = 0; i < list_size(lista); i++) {
        t_pid_por_bloque* elemento = (t_pid_por_bloque*)list_get(lista, i);
        if (elemento != NULL) {
            print_pid_por_bloque(elemento);
        } else {
            log_trace(logger_memoria,"Elemento en la posición %d es NULL.\n", i);
        }
    }
}


//
uint32_t calcular_base_proceso_fijas(uint32_t bloque, t_list* particiones){
    uint32_t base = 0;

    if (bloque < 0 || bloque >= list_size(particiones)) {
        return -1; 
    }

    // Suma los tamaños de las particiones anteriores al índice dado
    for (int i = 0; i < bloque; i++) {
        uint32_t tamanio_actual = atoi(list_get(particiones,i));
        base += tamanio_actual;
    }

    return base;
}



void inicializar_hilo(uint32_t pid, uint32_t tid, char* nombre_archivo){
    t_hilo* nuevo_hilo = malloc(sizeof(t_hilo));

    nuevo_hilo->tid = tid;
    nuevo_hilo->registros.PC = 0;
    nuevo_hilo->registros.AX = 0;
    nuevo_hilo->registros.BX = 0;
    nuevo_hilo->registros.CX = 0;
    nuevo_hilo->registros.DX = 0;
    nuevo_hilo->registros.EX = 0;
    nuevo_hilo->registros.FX = 0;
    nuevo_hilo->registros.GX = 0;
    nuevo_hilo->registros.HX = 0;

    nuevo_hilo->lista_de_instrucciones = list_create();
    leer_instrucciones_particiones_fijas(nombre_archivo,nuevo_hilo);
    asignar_hilo_a_proceso(nuevo_hilo,pid);
    
}



void asignar_hilo_a_proceso(t_hilo* hilo, uint32_t pid){
    bool encontrado = false;
    for (int i = 0; i < list_size(lista_miniPCBs); i++){
        t_miniPCB* miniPCB = list_get(lista_miniPCBs, i);

        if (miniPCB->pid == pid){
			list_add(miniPCB->hilos,hilo);
            log_trace(logger_memoria,"Se agrega tid %d a proceso : %d",hilo->tid,pid);
           // printf("Se agrega tid %d a proceso %d\n",hilo->tid, pid);
            encontrado = true;
        }
    }
    if(!encontrado){
        log_trace(logger_memoria,"No se asigno hilo ya que no se encuentra el proceso %d\n", pid);
    }

    
}