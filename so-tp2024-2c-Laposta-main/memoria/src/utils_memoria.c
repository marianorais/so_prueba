#include "../include/utils_memoria.h"

//Editar todas segun sea conveniente

/*---------------------------- KERNEL-------------------------*/

//Memoria deserializa lo enviado de Kernel
t_m_crear_proceso* deserializar_iniciar_proceso(t_list* lista_paquete) {
  
    // Crear la estructura del proceso y asignar espacio
    t_m_crear_proceso* crear_proceso = malloc(sizeof(t_m_crear_proceso));

    // Deserializar el PID
    crear_proceso->pid = *((uint32_t*) list_get(lista_paquete, 0));
    log_trace(logger_memoria,"Pid recibido: %d \n", crear_proceso->pid);

    // Deserializar el tamaño del proceso
    crear_proceso->tamanio_proceso = *((uint32_t*) list_get(lista_paquete, 1));
    log_trace(logger_memoria,"Tamanio recibido: %d \n", crear_proceso->tamanio_proceso);


    return crear_proceso;
}

//Memoria envia proceso creado a Kernel
void enviar_respuesta_iniciar_proceso(t_m_crear_proceso* crear_proceso ,int socket_kernel, op_code cod_ope) {
    t_paquete* paquete_crear_proceso;
 
    paquete_crear_proceso = crear_paquete(cod_ope);
 
     agregar_a_paquete(paquete_crear_proceso, &crear_proceso->pid,  sizeof(uint32_t));
     
    enviar_paquete(paquete_crear_proceso, socket_kernel);   
    log_trace(logger_memoria,"Proceso enviado: %i\n", crear_proceso->pid); 
    eliminar_paquete(paquete_crear_proceso);
    log_trace(logger_memoria,"PAQUETE ELIMINADO\n"); 
}

t_m_crear_hilo* deserializar_iniciar_hilo(t_list*  lista_paquete ){

    //Creamos una variable de tipo struct que ira guardando todo del paquete y le asignamos tamaño
    t_m_crear_hilo* crear_hilo = malloc(sizeof(t_m_crear_hilo));
    
    crear_hilo->pid = *((uint32_t*)list_get(lista_paquete, 0));
    log_trace(logger_memoria,"Pid recibido: %d \n", crear_hilo->pid);

    crear_hilo->tid = *((uint32_t*)list_get(lista_paquete, 1));
    log_trace(logger_memoria,"Tid recibido: %d \n", crear_hilo->tid);

    log_trace(logger_memoria,"Nombre del archivo: %s \n", (char*) list_get(lista_paquete, 2));
    crear_hilo->archivo_pseudocodigo = (char*) list_get(lista_paquete, 2);
    
    return crear_hilo;
}


//Memoria envia proceso creado a Kernel
void enviar_respuesta_iniciar_hilo(t_m_crear_hilo* crear_hilo ,int socket_kernel, op_code cod_ope) {
    t_paquete* paquete_crear_hilo;
 
    paquete_crear_hilo = crear_paquete(cod_ope);
 
    agregar_a_paquete(paquete_crear_hilo, &crear_hilo->pid,  sizeof(uint32_t));
    agregar_a_paquete(paquete_crear_hilo, &crear_hilo->tid,  sizeof(uint32_t));
     
    enviar_paquete(paquete_crear_hilo, socket_kernel);   
    log_trace(logger_memoria,"Hilo enviado: %i\n", crear_hilo->pid); 
    eliminar_paquete(paquete_crear_hilo);
    log_trace(logger_memoria,"PAQUETE ELIMINADO\n"); 
}


uint32_t deserializar_finalizar_proceso(t_list*  lista_paquete ){

    uint32_t proceso_a_finalizar;
    
    proceso_a_finalizar = *(uint32_t*)list_get(lista_paquete, 0);
    log_trace(logger_memoria,"Pid recibido: %d \n", proceso_a_finalizar);

    return proceso_a_finalizar;
}


void enviar_respuesta_finalizar_proceso(uint32_t pid_proceso_a_finalizar ,int socket_kernel, op_code cod_ope) {
    t_paquete* paquete_finalizar_proceso;
 
    paquete_finalizar_proceso = crear_paquete(cod_ope);
 
    agregar_a_paquete(paquete_finalizar_proceso, &pid_proceso_a_finalizar,  sizeof(uint32_t));
    
    enviar_paquete(paquete_finalizar_proceso, socket_kernel);   
    log_trace(logger_memoria,"Proceso enviado \n"); 
    eliminar_paquete(paquete_finalizar_proceso); 
}



uint32_t deserializar_finalizar_hilo(t_list*  lista_paquete ){

    uint32_t hilo_a_finalizar;
    
    hilo_a_finalizar = *(uint32_t*)list_get(lista_paquete, 0);
    log_trace(logger_memoria,"Tid recibido: %d \n", hilo_a_finalizar);

    return hilo_a_finalizar;
}


void enviar_respuesta_finalizar_hilo(uint32_t pid_proceso_a_finalizar ,uint32_t tid_proceso_a_finalizar,int socket_kernel,op_code cod_ope){
    t_paquete* paquete_finalizar_hilo;
 
    paquete_finalizar_hilo = crear_paquete(cod_ope);
 
    agregar_a_paquete(paquete_finalizar_hilo, &pid_proceso_a_finalizar,  sizeof(uint32_t));
    agregar_a_paquete(paquete_finalizar_hilo, &tid_proceso_a_finalizar,  sizeof(uint32_t));
    
    enviar_paquete(paquete_finalizar_hilo, socket_kernel);   
    log_trace(logger_memoria,"Hilo enviado \n"); 
    eliminar_paquete(paquete_finalizar_hilo); 
}

void enviar_confirmacion_memory_dump_a_kernel(op_code cod_ope,int socket_kernel){
    t_paquete* paquete_confirmacion_memory_dump;
 
    paquete_confirmacion_memory_dump = crear_paquete(cod_ope);
    int respuesta=PEDIDO_MEMORY_DUMP;
    agregar_a_paquete(paquete_confirmacion_memory_dump,&respuesta,sizeof(int));
    enviar_paquete(paquete_confirmacion_memory_dump, socket_kernel);   
    log_trace(logger_memoria,"Peticion confirmacion memory dump enviada \n"); 
    eliminar_paquete(paquete_confirmacion_memory_dump); 
}


/*---------------------------- CPU-------------------------*/

//Memoria deserializa/serializa lo enviado por Cpu


void deserializar_contexto(t_m_contexto* contexto,t_list*  lista_paquete ){

    //t_m_contexto* contexto = malloc(sizeof(t_m_contexto));
    
    contexto->pid = *(uint32_t*)list_get(lista_paquete, 0);
    log_trace(logger_memoria,"Pid recibido: %d \n", contexto->pid);
    
    contexto->tid = *(uint32_t*)list_get(lista_paquete, 1);
    log_trace(logger_memoria,"Tid recibido: %d \n", contexto->tid);

    contexto->registros.PC = *(uint32_t*)list_get(lista_paquete, 2);
    log_trace(logger_memoria,"Registro PC recibido: %d \n", contexto->registros.PC);

    contexto->registros.AX = *(uint32_t*)list_get(lista_paquete, 3);
    log_trace(logger_memoria,"Registro AX recibido: %d \n", contexto->registros.AX);

    contexto->registros.BX = *(uint32_t*)list_get(lista_paquete, 4);
    log_trace(logger_memoria,"Registro BX recibido: %d \n", contexto->registros.BX);

    contexto->registros.CX = *(uint32_t*)list_get(lista_paquete, 5);
    log_trace(logger_memoria,"Registro CX recibido: %d \n", contexto->registros.CX);

    contexto->registros.DX = *(uint32_t*)list_get(lista_paquete, 6);
    log_trace(logger_memoria,"Registro DX recibido: %d \n", contexto->registros.DX);

    contexto->registros.EX = *(uint32_t*)list_get(lista_paquete, 7);
    log_trace(logger_memoria,"Registro EX recibido: %d \n", contexto->registros.EX);

    contexto->registros.FX = *(uint32_t*)list_get(lista_paquete, 8);
    log_trace(logger_memoria,"Registro FX recibido: %d \n", contexto->registros.FX);

    contexto->registros.GX = *(uint32_t*)list_get(lista_paquete, 9);
    log_trace(logger_memoria,"Registro GX recibido: %d \n", contexto->registros.GX);

    contexto->registros.HX = *(uint32_t*)list_get(lista_paquete, 10);
    log_trace(logger_memoria,"Registro HX recibido: %d \n", contexto->registros.HX);

    contexto->base = *(uint32_t*)list_get(lista_paquete, 11);
    log_trace(logger_memoria,"Registro base recibido: %d \n", contexto->base);

    contexto->limite = *(uint32_t*)list_get(lista_paquete, 12);
    log_trace(logger_memoria,"Registro limite recibido: %d \n", contexto->limite);

   // return contexto;
}





void enviar_respuesta_contexto(t_m_contexto* pcbproceso, int socket_cpu) {
    t_paquete* paquete_cpu = crear_paquete(SOLICITUD_CONTEXTO_RTA); // Tipo de paquete que indica envío a CPU

    // Agregar información del PCB al paquete
    agregar_a_paquete(paquete_cpu, &pcbproceso->pid, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->tid, sizeof(uint32_t));

    // Agregar los registros de la CPU al paquete individualmente
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros.PC, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros.AX, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros.BX, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros.CX, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros.DX, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros.EX, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros.FX, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros.GX, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros.HX, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->base, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->limite, sizeof(uint32_t));

    // Enviar el paquete a la CPU
    enviar_paquete(paquete_cpu, socket_cpu); 
    log_trace(logger_memoria,"Contexto enviado para pid %d tid %d\n", pcbproceso->pid,pcbproceso->tid); 
    // Liberar recursos del paquete
    eliminar_paquete(paquete_cpu);
}


t_proceso_memoria* deserializar_solicitud_instruccion(t_list*  lista_paquete ){

    t_proceso_memoria* proxima_instruccion = malloc(sizeof(t_proceso_memoria));
    
    proxima_instruccion->pid = *((uint32_t*)list_get(lista_paquete, 0));
    log_trace(logger_memoria,"Pid recibido: %d \n", proxima_instruccion->pid);
    proxima_instruccion->tid = *((uint32_t*)list_get(lista_paquete, 1));
    log_trace(logger_memoria,"Tid recibido: %d \n", proxima_instruccion->tid);
    proxima_instruccion->program_counter = *((uint32_t*)list_get(lista_paquete, 2));
    log_trace(logger_memoria,"Program counter: %d \n", proxima_instruccion->program_counter);

    return proxima_instruccion;
}


void enviar_respuesta_instruccion(char* proxima_instruccion ,int socket_cpu) {
    t_paquete* paquete_instruccion;
 
    paquete_instruccion = crear_paquete(SOLICITUD_INSTRUCCION_RTA);

    agregar_a_paquete(paquete_instruccion, proxima_instruccion,  strlen(proxima_instruccion) + 1);          
    
    enviar_paquete(paquete_instruccion, socket_cpu);   
    log_trace(logger_memoria,"Instruccion enviada %s\n", proxima_instruccion); 
    log_trace(logger_memoria,"LONGITUD DE LA INTRUCCION: %d\n", strlen(proxima_instruccion)); 
    eliminar_paquete(paquete_instruccion);
}


t_escribir_leer* deserializar_read_memoria(t_list*  lista_paquete ){

    t_escribir_leer* peticion_valor = malloc(sizeof(t_escribir_leer));
    
    peticion_valor->pid = *(uint32_t*)list_get(lista_paquete, 0);
    log_trace(logger_memoria,"Pid recibido: %d \n", peticion_valor->pid);

    peticion_valor->tid = *(uint32_t*)list_get(lista_paquete, 1);
    log_trace(logger_memoria,"Tid recibido: %d \n", peticion_valor->tid);
    
    peticion_valor->direccion_fisica = *(uint32_t*)list_get(lista_paquete, 2);
    log_trace(logger_memoria,"Direccion fisica: %d \n", peticion_valor->direccion_fisica);

    //peticion_valor->tamanio = *(uint32_t*)list_get(lista_paquete, 3);
    //log_trace(logger_memoria,"Tamanio proceso: %d \n", peticion_valor->tamanio);

    return peticion_valor;
}


void enviar_respuesta_read_memoria(uint32_t pid, uint32_t respuesta_leer, int socket_cpu, op_code cod_ope) {
    t_paquete* paquete_valor;

    // Verificar que respuesta_leer no sea NULL
    if (respuesta_leer == -1) {
        log_trace(logger_memoria,stderr, "Error: respuesta_leer es -1\n");
        return;
    }


    paquete_valor = crear_paquete(cod_ope);

    //uint32_t tamanio_respuesta_leer = (longitud * sizeof(char)) + 1;
    uint32_t tamanio_respuesta_leer = 4;

    agregar_a_paquete(paquete_valor, &pid, sizeof(uint32_t)); 
    agregar_a_paquete(paquete_valor, &tamanio_respuesta_leer, sizeof(uint32_t)); 
    agregar_a_paquete(paquete_valor, &respuesta_leer, sizeof(uint32_t));          

    log_trace(logger_memoria,"respuesta_leer: %d , tamanio %d \n", respuesta_leer, tamanio_respuesta_leer); 
    
    enviar_paquete(paquete_valor, socket_cpu);   
    log_trace(logger_memoria,"Se envió respuesta de lectura \n"); 

    eliminar_paquete(paquete_valor);
}




t_escribir_leer* deserializar_write_memoria(t_list*  lista_paquete){

    t_escribir_leer* peticion_guardar = malloc(sizeof(t_escribir_leer));
    
    peticion_guardar->pid = *(uint32_t*)list_get(lista_paquete, 0);
    log_trace(logger_memoria,"Pid recibido: %d \n", peticion_guardar->pid);

    peticion_guardar->tid = *(uint32_t*)list_get(lista_paquete, 1);
    log_trace(logger_memoria,"Tid recibido: %d \n", peticion_guardar->tid);
    
    peticion_guardar->direccion_fisica = *(uint32_t*)list_get(lista_paquete, 2);
    log_trace(logger_memoria,"Direccion fisica: %d \n", peticion_guardar->direccion_fisica);

    peticion_guardar->valor = *(uint32_t*)list_get(lista_paquete, 3);
    log_trace(logger_memoria,"Valor: %d \n", peticion_guardar->valor);
log_trace(logger_memoria, "Valor deser. WRITE: %d \n", peticion_guardar->valor);
    

    return peticion_guardar;
}






void enviar_respuesta_write_memoria(uint32_t pid, int socket_cliente, op_code cod_ope){
    t_paquete* paquete_valor;

    paquete_valor = crear_paquete(cod_ope);

    agregar_a_paquete(paquete_valor, &pid,  sizeof(uint32_t));

    enviar_paquete(paquete_valor, socket_cliente);
    log_trace(logger_memoria,"Se envio respuesta de guardado \n"); 
   eliminar_paquete(paquete_valor);
}

void enviar_respuesta_actualizar_contexto(t_m_contexto* contexto ,int socket_cpu, op_code cod_ope) {
    t_paquete* paquete_contexto;
 
    paquete_contexto = crear_paquete(cod_ope);
 
    agregar_a_paquete(paquete_contexto, &contexto->pid,  sizeof(uint32_t));
    agregar_a_paquete(paquete_contexto, &contexto->tid,  sizeof(uint32_t));
     
    enviar_paquete(paquete_contexto, socket_cpu);   
    eliminar_paquete(paquete_contexto);
    log_trace(logger_memoria,"PAQUETE ELIMINADO\n"); 
}

/*---------------------------- FILE SYSTEM-------------------------*/

void enviar_creacion_memory_dump(uint32_t tamanio_nombre_archivo, char* nombre_archivo ,uint32_t tamanio_contenido,char* contenido, int socket_fs){
    t_paquete* paquete_creacion_memory_dump;
 
    paquete_creacion_memory_dump = crear_paquete(CREACION_DUMP);
 
    agregar_a_paquete(paquete_creacion_memory_dump, nombre_archivo,  tamanio_nombre_archivo);
    agregar_a_paquete(paquete_creacion_memory_dump, &tamanio_contenido,  sizeof(uint32_t));
    agregar_a_paquete(paquete_creacion_memory_dump, contenido,  tamanio_contenido);
    
    enviar_paquete(paquete_creacion_memory_dump, socket_fs);   
    log_trace(logger_memoria,"Peticion creacion memory dump enviada \n"); 
    eliminar_paquete(paquete_creacion_memory_dump); 
}
