#include "../include/particion_dinamica.h"


//=================================== Funciones para la finalizacion de un proceso =============================

int crear_proceso(uint32_t proceso_pid, uint32_t tamanio_proceso){

    int respuesta;

    if (strcmp(cfg_memoria->ESQUEMA, "DINAMICAS") == 0){
        respuesta = crear_proceso_dinamico(proceso_pid, tamanio_proceso);
    }else{
        if (strcmp(cfg_memoria->ESQUEMA, "FIJAS") == 0)
            respuesta = crear_proceso_fijas(tamanio_proceso,lista_particiones,proceso_pid);
    }
    return respuesta;
}


//Funcion que crea las estructuras del proceso
int crear_proceso_dinamico(uint32_t proceso_pid, uint32_t tamanio_proceso){

    log_trace(logger_memoria,"ENTRA CREAR PROCESO DINAMICO\n");
    log_trace(logger_memoria, "Iniciando estructura dinamica \n");
    
    int resutado_crear_proceso = asignar_memoria(proceso_pid, tamanio_proceso);
    
    return resutado_crear_proceso;
}




int asignar_memoria(uint32_t proceso_pid, uint32_t tamanio_proceso){

    t_particion_dinamica *particion_resultante = NULL;


    // Selecciona la partición según el algoritmo
    if (strcmp(cfg_memoria->ALGORITMO_BUSQUEDA, "FIRST") == 0) {
        particion_resultante = buscar_first_fit(tamanio_proceso);
        log_trace(logger_memoria, "Se eligio la primer particion posible \n");

    } else if (strcmp(cfg_memoria->ALGORITMO_BUSQUEDA, "BEST") == 0) {
        particion_resultante = buscar_best_fit(tamanio_proceso);
        log_trace(logger_memoria, "Se eligio la mejor particion posible \n");

    } else if (strcmp(cfg_memoria->ALGORITMO_BUSQUEDA, "WORST") == 0) {
        particion_resultante = buscar_worst_fit(tamanio_proceso);
        log_trace(logger_memoria, "Se eligio la peor particion posible \n");
    }


    // Si es null no hay memoria
    if (particion_resultante == NULL) {
        log_error(logger_memoria, "No hay memoria \n");
        return INICIAR_PROCESO_RTA_ERROR_SIN_ESPACIO; 
    }

    particion_resultante->pid = proceso_pid;

    // Divido la partición si es más grande que el tamaño del proceso
    dividir_particion(particion_resultante, tamanio_proceso);

    return INICIAR_PROCESO_RTA_OK;
}


//============================= Algoritmos de busqueda =================================

//Funcion que busca la primer particion
t_particion_dinamica *buscar_first_fit(uint32_t tamanio_proceso){

    t_particion_dinamica *particion;

    //Recorremos la lista enlazada de particiones
    for (int i = 0; i < list_size(lista_particiones_dinamicas); i++){

        particion =  list_get(lista_particiones_dinamicas, i);

        //Verificamos que la particion obtenida este libre y sea >= al proceso
        if (!particion->ocupado && particion->tamanio >= tamanio_proceso) {
            return particion;
        }
    }
    return NULL;
}


//Funcion que busca la mejor particion
t_particion_dinamica *buscar_best_fit(uint32_t tamanio_proceso){

    t_particion_dinamica *particion;

    //iniciamos la variable mejor en Null
    t_particion_dinamica *mejor_particion = NULL;
    //Recorremos la lista de particiones comparando en cada iteracion
    for (int i = 0; i < list_size(lista_particiones_dinamicas); i++){
        particion = list_get(lista_particiones_dinamicas, i);

        //Verificamos que la particion obtenida este libre y sea >= al proceso
        if (!particion->ocupado && particion->tamanio >= tamanio_proceso) {

            //Verificamos si todavia no hay una mejor o si el tamaño particion actual es menor a la mejor actual 
            if (mejor_particion == NULL || particion->tamanio < mejor_particion->tamanio) {
                mejor_particion = particion;
            }
        }
    }

    //Retornamos la mejor o si no hay Null
    return mejor_particion;
}


//Funcion que busca la peor particion
t_particion_dinamica *buscar_worst_fit(uint32_t tamanio_proceso){

    t_particion_dinamica *particion;

    //iniciamos la variable mejor en Null
    t_particion_dinamica *peor_particion = NULL;

    //Recorremos la lista de particiones comparando en cada iteracion
    for (int i = 0; i < list_size(lista_particiones_dinamicas); i++){
        
        particion = list_get(lista_particiones_dinamicas, i);

        //Verificamos que la particion obtenida este libre y sea >= al proceso
        if (!particion->ocupado && particion->tamanio >= tamanio_proceso){

            //Verificamos si todavia no hay una peor o si el tamaño particion actual es mayor a la peor actual 
            if (peor_particion == NULL || particion->tamanio > peor_particion->tamanio) {
                peor_particion = particion;
            }
        }
    }

    //Retornamos la peor o si no hay Null
    return peor_particion;
}




//Funcion que va diviendo el espacio deacuerdo al tamaño del proceso
void dividir_particion(t_particion_dinamica* particion, uint32_t tamanio_proceso) {
    
    //Si la partición tiene exactamente el tamaño necesario, no la dividimos
    if (particion->tamanio == tamanio_proceso){

        particion->tamanio = tamanio_proceso;
        particion->ocupado = true;

        t_miniPCB* proceso = malloc(sizeof(t_miniPCB));
        proceso->pid = particion->pid;
        proceso->base = particion->inicio;
        proceso->limite = tamanio_proceso + particion->inicio;
        proceso->hilos = list_create();

        pthread_mutex_lock(&mutex_lista_miniPCBs);
        list_add(lista_miniPCBs, proceso);
        pthread_mutex_unlock(&mutex_lista_miniPCBs);

        //list_add(lista_particiones_dinamicas, particion); //VER
        log_trace(logger_memoria, "Asiganada una particion exacta al PID - %d \n", particion->pid);
        
    }else{

        // 1. Se crea una particion nueva libre con el espacio sobrante del proceso
        
        log_trace(logger_memoria, "La particion es grande para el proceso \n");
        log_trace(logger_memoria, "Ajustando el tamaño a la particion");

        //Creamos una nueva partición para la parte libre
        t_particion_dinamica* nueva_particion = malloc(sizeof(t_particion_dinamica));
        nueva_particion->inicio = particion->inicio + tamanio_proceso;      //Empieza a partir del fin del proceso anterior
        nueva_particion->tamanio = particion->tamanio - tamanio_proceso;    //La nueva particion tiene tamaño = total- usado
        nueva_particion->ocupado = false;                                   //La nueva particion esta libre
        nueva_particion->pid = -1;                                           //No tiene proceso asignado (ver si va 0)
        nueva_particion->tid = -1;                                           //No tiene hilo asignado (ver si va 0)

        log_trace(logger_memoria, "Creada una particion libre con el sobrante \n");
        log_trace(logger_memoria, "Tamanio de la nueva particion libre: %d", nueva_particion->tamanio);

        // 2. Se crea el proceso y sus estructuras con su tamaño solicitado

        t_miniPCB* proceso = malloc(sizeof(t_miniPCB));
        proceso->pid = particion->pid;
        proceso->base = particion->inicio;
        proceso->limite = tamanio_proceso + particion->inicio;
        proceso->hilos = list_create();

        particion->tamanio = tamanio_proceso;
        particion->ocupado = true;
        
        // 3. Se añade el proceso a la lista de procesos y la nueva particion a la lista de particiones

        pthread_mutex_lock(&mutex_lista_miniPCBs);
        list_add(lista_miniPCBs, proceso);
        pthread_mutex_unlock(&mutex_lista_miniPCBs);
        
        pthread_mutex_lock(&mutex_lista_particiones_dinamicas);
        list_add(lista_particiones_dinamicas, nueva_particion);
        pthread_mutex_unlock(&mutex_lista_particiones_dinamicas);
    }

    
}







char* escribir_memoria(uint32_t proceso_pid, uint32_t direccion_fisica, char* valor, uint32_t tamanio_a_escribir) {
    
    //Se espera que tamanio_a_escribir sea 4 bytes.
    if (tamanio_a_escribir != 4){
        log_error(logger_memoria, "ERROR: Se espera escribir 4 bytes.");
    }

    //Reservar espacio para la respuesta
    char* escrito;

    //Busca la partición asociada al proceso y al bloque de memoria.
    t_particion_dinamica* particion = busco_particion_dinamica_por_PID(proceso_pid);

    if (particion == NULL){
        log_error(logger_memoria, "ERROR: Partición no encontrada");
    }

    //Verificar que hay suficiente espacio desde la dirección física en la partición.
    uint32_t espacio_disponible = particion->inicio + particion->tamanio - direccion_fisica;

    if (espacio_disponible < tamanio_a_escribir){
        log_error(logger_memoria, "ERROR: Espacio insuficiente en la partición.");
    }

    //Copiar los 4 bytes del valor en la memoria.
    
    pthread_mutex_lock(&mutex_memoria);
    memcpy(memoria_usuario + direccion_fisica, valor, tamanio_a_escribir);
    pthread_mutex_unlock(&mutex_memoria);
    escrito = "OK";

    return escrito;
}




char* leer_memoria(uint32_t proceso_pid, uint32_t direccion_fisica, uint32_t tamanio_a_leer) {
    
    //Se espera que tamanio_a_leer sea 4 bytes.
    if (tamanio_a_leer != 4){
        log_error(logger_memoria, "ERROR: Se espera leer 4 bytes.");
    }

    //Reservar espacio para los 4 bytes que se van a leer.
    char* leido = (char*) malloc(tamanio_a_leer);

    //Busca la partición asociada al proceso y al bloque de memoria.
    t_particion_dinamica* particion = busco_particion_dinamica_por_PID(proceso_pid);

    if (particion == NULL){

        free(leido);
        log_error(logger_memoria, "ERROR: Partición no encontrada");
    }

    //Verificar que hay suficiente espacio desde la dirección física en la partición.
    uint32_t espacio_disponible = particion->inicio + particion->tamanio - direccion_fisica;

    if (espacio_disponible < tamanio_a_leer){
        free(leido);
        log_error(logger_memoria, "ERROR: Espacio insuficiente en la partición.");
    }

    //Copiar los 4 bytes de la memoria a la variable leída.
    memcpy(leido, memoria_usuario + direccion_fisica, tamanio_a_leer);

    return leido;
}





//=================================== Funciones para la finalizacion de un proceso =============================



//Funcion que devuelve el indice de un proceso a partir de un PID
int busco_indice_particion_dinamica_por_PID(uint32_t proceso_pid){

    log_trace(logger_memoria, "Buscando el indice de la particion por PID");

    //Recorremos la lista de particiones
    for (int i = 0; i < list_size(lista_particiones_dinamicas); i++){

        t_particion_dinamica *particion = list_get(lista_particiones_dinamicas, i);

        //Si el PID de la partición es el mismo, retornamos el índice
        if (proceso_pid == particion->pid){
            return i;
        }
    }

    log_error(logger_memoria, "No se encontro el indice de la Particion de PID - %d", proceso_pid);
    abort();
}

//Funcion que devuelve una particion dinamica segun un PID
t_particion_dinamica *busco_particion_dinamica_por_PID(uint32_t proceso_pid){

    log_trace(logger_memoria, "Buscando la particion por PID");

    t_particion_dinamica *particion;

    //Recorremos la lista enlazada que contiene particiones
    for (int i = 0; i < list_size(lista_particiones_dinamicas); i++){
    
        particion = list_get(lista_particiones_dinamicas, i);

        //Si el id de la particion es el mismo, la retorna
        if (proceso_pid == particion->pid){
            return particion;
        }

    }

    log_error(logger_memoria, "No se encontro la Particion relacionada al PID - %d", proceso_pid);
    abort();
}


//Funcion que devuelve un proceso segun un PID
t_miniPCB *busco_proceso_por_PID(uint32_t proceso_pid){

    log_trace(logger_memoria, "Buscando el proceso por PID");

    t_miniPCB *proceso;

    //Recorremos la lista que contiene la lista de procesos
    for (int i = 0; i < list_size(lista_miniPCBs); i++){

        //Sacamos un proceso de la lista
        proceso = list_get(lista_miniPCBs, i);

        //Si el id del proceso es el mismo que el proceso buscado, la retorna
        if (proceso_pid == proceso->pid)
            return proceso;
    }

    log_error(logger_memoria, "PID - %d No se encontro el proceso", proceso_pid);
    abort();
}




t_hilo *busco_hilo_por_TID(uint32_t hilo_tid, t_miniPCB* proceso){

    log_trace(logger_memoria, "Buscando el hilo por TID");

    t_hilo *hilo;

    //Recorremos la lista que contiene la lista de hilo
    for (int i = 0; i < list_size(proceso->hilos); i++){

        //Sacamos un hilo de la lista
        hilo = list_get(proceso->hilos, i);

        //Si el id del hilo es el mismo que el hilo buscado, la retorna
        if (hilo_tid == hilo->tid)
            return hilo;
    }

    log_error(logger_memoria, "No se encontro el hilo TID - %d", hilo_tid);
    abort();
}





// Función para unificar particiones libres adyacentes
void unificar_particiones_dinamicas(int indice_particion){

    t_particion_dinamica *particion_actual = list_get(lista_particiones_dinamicas, indice_particion);

    // 1. Unificación hacia adelante (si hay siguiente)
    if (indice_particion +1 < list_size(lista_particiones_dinamicas)) {

        t_particion_dinamica *particion_siguiente = list_get(lista_particiones_dinamicas, indice_particion + 1);

        //Si la siguiente esta libre
        if (!particion_siguiente->ocupado){

            log_trace(logger_memoria, "Unificando con la particion siguiente libre.");
            
            // Aumentamos el tamaño de la partición actual
            particion_actual->tamanio += particion_siguiente->tamanio;

            // Liberamos la partición siguiente que ha sido unificada
            list_remove_and_destroy_element(lista_particiones_dinamicas, indice_particion + 1, free);
        }
    }


    // 2. Unificación hacia atrás (si hay anterior)
    if (indice_particion - 1 >= 0){
        
        t_particion_dinamica *particion_anterior = list_get(lista_particiones_dinamicas, indice_particion -1);

        // Si la anterior está libre
        if (!particion_anterior->ocupado) {

            log_trace(logger_memoria, "Unificando con la particion anterior libre.");

            // Aumentamos el tamaño de la partición anterior
            particion_anterior->tamanio += particion_actual->tamanio;

            // Liberamos la partición actual que ha sido unificada
            list_remove_and_destroy_element(lista_particiones_dinamicas, indice_particion, free);
        }
    }
}




//Funcion que en base al id de un proceso finalizamos sus estructuras
void finalizar_proceso_dinamico(uint32_t proceso_pid){

    log_trace(logger_memoria, "Liberacion del proceso PID %i", proceso_pid);

    int indice_particion = busco_indice_particion_dinamica_por_PID(proceso_pid);
    t_particion_dinamica *particion = busco_particion_dinamica_por_PID(proceso_pid);
    //t_miniPCB *proceso = busco_proceso_por_PID(proceso_pid);


    log_trace(logger_memoria, "Log Obligatorio: \n");
    log_trace(logger_memoria, "Destruccion de particion: \n");

    particion->ocupado = false;

    unificar_particiones_dinamicas(indice_particion);
    
    eliminar_proceso_de_lista(proceso_pid);
}


//Funcion que a partir de PID finaliza el proceso sogun el tipo de config
void finalizar_proceso(uint32_t proceso_pid){


    if (strcmp(cfg_memoria->ESQUEMA, "DINAMICAS") == 0){
        finalizar_proceso_dinamico(proceso_pid);
    }else{
        if (strcmp(cfg_memoria->ESQUEMA, "FIJAS") == 0)
            finalizar_proceso_fijas(proceso_pid);
    }

}