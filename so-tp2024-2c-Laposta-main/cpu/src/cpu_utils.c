#include"../include/cpu_utils.h"

int tamanioParams;
int tamanioInterfaces;
t_proceso* proceso_actual;

bool flag_sementation_fault;
instr_t* fetch(int conexion, t_proceso* proceso){
    
    pedir_instruccion(proceso, conexion); 
    log_info(logger_cpu, "TID:%d -FETCH- Program Counter: %d", proceso->tid,proceso->registros_cpu.PC); //LOG OBLIGATORIO   
    sem_wait(&sem_valor_instruccion);
    return prox_inst;
}

tipo_instruccion decode(instr_t* instr, int conexion_memo){
    log_trace(logger_cpu, "EL codigo de instrucción es %d ",instr->id);
     
    return instr->id  ; 
  
}


void execute(instr_t* inst,tipo_instruccion tipo_inst, t_proceso* proceso, int conexion, int socket_dispatch, int socket_interrupt){
    
        switch(tipo_inst){
            case SET:
            {      
                log_info(logger_cpu, "TID: %u - Ejecutando: SET - %s %s", proceso->tid,inst->param1,inst->param2); //LOG OBLIGATORIO           
                set(inst->param1, inst->param2, proceso);
                break;
            }
            case SUM:
            {
                log_info(logger_cpu, "TID: %u - Ejecutando: SUM - %s %s", proceso->tid,inst->param1,inst->param2); //LOG OBLIGATORIO
                sum(inst->param1, inst->param2, proceso);
                break;
            }
            case SUB:
            {
                log_info(logger_cpu, "TID: %u - Ejecutando: SUB - %s %s", proceso->tid,inst->param1,inst->param2); //LOG OBLIGATORIO
                sub(inst->param1, inst->param2, proceso);
                break;
            }            

            case JNZ:
            {
                log_info(logger_cpu, "TID: %u - Ejecutando: JNZ - %s %s", proceso->tid,inst->param1,inst->param2); //LOG OBLIGATORIO
                jnz(inst->param1, inst->param2,proceso);
                break;
            }        

            case READ_MEM:
            {
                log_info(logger_cpu, "TID: %u - Ejecutando: READ_MEM - %s %s", proceso->tid,inst->param1,inst->param2); //LOG OBLIGATORIO
                read_mem(inst->param1,inst->param2,proceso,conexion); //proceso estaba como proceso_actual, revisar esto ya que en los otros estan como proceso
                break;
            }   

            case WRITE_MEM:
            {
                log_info(logger_cpu, "TID: %u - Ejecutando: WRITE_MEM - %s %s", proceso->tid,inst->param1,inst->param2); //LOG OBLIGATORIO
                write_mem(inst->param1,inst->param2,proceso,conexion);//proceso estaba como proceso_actual, revisar esto ya que en los otros estan como proceso
                break;
            }   

            case LOG:
            {
                log_info(logger_cpu, "TID: %u - Ejecutando: LOG - %s", proceso->tid,inst->param1); //LOG OBLIGATORIO
                loguear(inst->param1);
                break;
            }

            // SYSCALLS:
            case DUMP_MEMORY:
            {        
                proceso->registros_cpu.PC += 1;
                enviar_contexto_a_memoria(proceso,conexion);

                log_info(logger_cpu, "TID: %u - Ejecutando: DUMP_MEMORY", proceso->tid);                
             sem_wait(&semaforo_sincro_contexto_syscall);
                enviar_dump_memory_a_kernel(socket_dispatch);
                break;
            }
            case IO:
            {   
                proceso->registros_cpu.PC += 1;
                enviar_contexto_a_memoria(proceso,conexion);

                log_info(logger_cpu, "TID: %u - Ejecutando: IO - %s", proceso->tid, inst->param1); //LOG OBLIGATORIO
                sem_wait(&semaforo_sincro_contexto_syscall);
                enviar_io_a_kernel(inst->param1,socket_dispatch);
            
                break;
            }
            case PROCESS_CREATE:
            {
                proceso->registros_cpu.PC += 1;
                enviar_contexto_a_memoria(proceso,conexion);
                log_info(logger_cpu, "TID: %u - Ejecutando: PROCESS_CREATE - %s %s %s", proceso->tid,inst->param1, inst->param2, inst->param3); //LOG OBLIGATORIO
                sem_wait(&semaforo_sincro_contexto_syscall);
                enviar_process_create_a_kernel(inst->param1, inst->param2, inst->param3, socket_dispatch);
            
                
                break;
            }
            case THREAD_CREATE:
            {
                proceso->registros_cpu.PC += 1;
                enviar_contexto_a_memoria(proceso,conexion);
                log_info(logger_cpu, "TID: %u - Ejecutando: THREAD_CREATE - %s %s", proceso->tid,inst->param1,inst->param2); //LOG OBLIGATORIO
                sem_wait(&semaforo_sincro_contexto_syscall);
                enviar_thread_create_a_kernel(inst->param1, inst->param2, socket_dispatch);
               
                break;
            }
            case THREAD_JOIN:
            {
                proceso->registros_cpu.PC += 1;
                enviar_contexto_a_memoria(proceso,conexion);
                log_info(logger_cpu, "TID: %u - Ejecutando: THREAD_JOIN - %s", proceso->tid, inst->param1); //LOG OBLIGATORIO
                 sem_wait(&semaforo_sincro_contexto_syscall);
                 enviar_thread_join_a_kernel(inst->param1, socket_dispatch);
            
                break;
            }
            case THREAD_CANCEL:
            {
                proceso->registros_cpu.PC += 1;
                enviar_contexto_a_memoria(proceso,conexion);
                log_info(logger_cpu, "TID: %u - Ejecutando: THREAD_CANCEL - %s", proceso->tid, inst->param1); //LOG OBLIGATORIO
                sem_wait(&semaforo_sincro_contexto_syscall);
                enviar_thread_cancel_a_kernel(inst->param1, socket_dispatch);

              
                break;
            }
            case MUTEX_CREATE:
            {
                proceso->registros_cpu.PC += 1;
                enviar_contexto_a_memoria(proceso,conexion);
                log_info(logger_cpu, "TID: %u - Ejecutando: MUTEX_CREATE - %s", proceso->tid, inst->param1); //LOG OBLIGATORIO
                sem_wait(&semaforo_sincro_contexto_syscall); 
                enviar_mutex_create_a_kernel(inst->param1, socket_dispatch);
               
                break;
            }
            case MUTEX_LOCK:
            {   
                proceso->registros_cpu.PC += 1;
                enviar_contexto_a_memoria(proceso,conexion);
                log_info(logger_cpu, "TID: %u - Ejecutando: MUTEX_LOCK - %s", proceso->tid, inst->param1); //LOG OBLIGATORIO
                sem_wait(&semaforo_sincro_contexto_syscall);
                enviar_mutex_lock_a_kernel(inst->param1, socket_dispatch); 
               
                break;
            }
            case MUTEX_UNLOCK:
            {
                proceso->registros_cpu.PC += 1;
                enviar_contexto_a_memoria(proceso,conexion);
                log_info(logger_cpu, "TID: %u - Ejecutando: MUTEX_UNLOCK - %s", proceso->tid, inst->param1); //LOG OBLIGATORIO
                sem_wait(&semaforo_sincro_contexto_syscall);
                enviar_mutex_unlock_a_kernel(inst->param1, socket_dispatch);
              
                break;
            }
            case THREAD_EXIT:
            {enviar_contexto_a_memoria(proceso,conexion);
                log_info(logger_cpu, "TID: %u - Ejecutando: THREAD_EXIT", proceso->tid);
                sem_wait(&semaforo_sincro_contexto_syscall);
                enviar_thread_exit_a_kernel(socket_dispatch);
              
                break;
            }                
            case PROCESS_EXIT:
            {enviar_contexto_a_memoria(proceso,conexion);
                log_info(logger_cpu, "TID: %u - Ejecutando: PROCESS_EXIT", proceso->tid);
                sem_wait(&semaforo_sincro_contexto_syscall);
                enviar_process_exit_a_kernel(socket_dispatch);               
                break;
            }            

            default:
                log_trace(logger_cpu, "Hubo un error: instrucción no encontrada");
        }

}
void enviar_fin_quantum_a_kernel(    t_proceso *proceso,int socket){
    t_paquete* paquete_fin_quantum;
    paquete_fin_quantum = crear_paquete(FIN_DE_QUANTUM);
    agregar_a_paquete(paquete_fin_quantum, &(proceso->pid), sizeof(uint32_t));
    agregar_a_paquete(paquete_fin_quantum, &(proceso->tid), sizeof(uint32_t));
  
    enviar_paquete(paquete_fin_quantum, socket);
    log_trace(logger_cpu, "TIEMPO DE QUANTUM TERMINADO - PID: %d, TID: %d", proceso->pid, proceso->tid);
    eliminar_paquete(paquete_fin_quantum);
    pthread_mutex_lock(&mutex_proceso_actual);
    if (proceso_actual != NULL) {
                free(proceso_actual);
                 proceso_actual = NULL;
                } 
    log_trace(logger_cpu, "proceso desalojado");
    
    pthread_mutex_unlock(&mutex_proceso_actual);
}

void check_interrupt(int conexion_kernel){
    log_trace(logger_cpu,"ENTRO EN CHECK INTERRUPT\n");    
  
    pthread_mutex_lock(&mutex_interrupcion_kernel);
    
    if(interrupcion_kernel && proceso_actual != NULL){
        log_trace(logger_cpu,"ENTRO EN IF DEL  CHECK INTERRUPT\n");
        enviar_contexto_a_memoria(proceso_actual,socket_memoria);
        enviar_fin_quantum_a_kernel(proceso_actual,conexion_kernel );
      
        interrupcion_kernel = false;
    }
    pthread_mutex_unlock(&mutex_interrupcion_kernel);
    
}

void pedir_instruccion(t_proceso* proceso,int conexion){  
    
    t_paquete* paquete_pedido_instruccion;
    paquete_pedido_instruccion = crear_paquete(SOLICITUD_INSTRUCCION);
    log_trace(logger_cpu,"SOLICITANDO INSTRUCICON A MEM: PID=%d, TID:%d",proceso->pid,proceso->tid) ;  
    agregar_a_paquete(paquete_pedido_instruccion,  &(proceso->pid),  sizeof(uint32_t));
    agregar_a_paquete(paquete_pedido_instruccion, &(proceso->tid),  sizeof(uint32_t));
    agregar_a_paquete(paquete_pedido_instruccion,  &(proceso->registros_cpu.PC),  sizeof(uint32_t));  
        
    enviar_paquete(paquete_pedido_instruccion, conexion); 
    eliminar_paquete(paquete_pedido_instruccion);
}

//////////////////////////////////////// INSTRUCCIONES //////////////////////////////////////////
void set(char* registro, char* valor_char, t_proceso* proceso){
    char *endptr;     
    registros registro_elegido = identificarRegistro(registro);
    //uint32_t valor = strtoul(valor_char, &endptr, 10);// Convertir la cadena a uint32_t
    uint32_t valor = atoi(valor_char);
    //pthread_mutex_lock(&mutex_proceso_actual);
    switch(registro_elegido){
        case PC:
        {
          proceso->registros_cpu.PC = valor;
            break;
        }
        case AX:
        {
          proceso->registros_cpu.AX = valor;
            break;
        }
        case BX:
        {
          proceso->registros_cpu.BX = valor;
            break;
        }
        case CX:
        {
          proceso->registros_cpu.CX = valor;
            break;
        }
        case DX:
        {
          proceso->registros_cpu.DX = valor;
            break;
        }
        case EX:
        {
          proceso->registros_cpu.EX = valor;
            break;
        }
        case FX: 
        {
          proceso->registros_cpu.FX = valor;
            break;
        }
        case GX:
        {
          proceso->registros_cpu.GX = valor;
            break;
        }
        case HX:
        {
          proceso->registros_cpu.HX = valor;
            break;
        }
        case base:
        {
          proceso->registros_cpu.base = valor;
            break;
        }
        case limite:
        {
          proceso->registros_cpu.limite = valor;
            break;
        }
        
        default:
        log_trace(logger_cpu, "El registro no existe");
    }
   // pthread_mutex_unlock(&mutex_proceso_actual);

    //proceso->pcb->registros_cpu.AX;
   // registro = valor;
}

void sum(char* registro_destino, char* registro_origen, t_proceso* proceso){
    registros id_registro_destino = identificarRegistro(registro_destino);
    registros id_registro_origen = identificarRegistro(registro_origen);

    uint32_t valor_reg_destino = obtenerValorActualRegistro(id_registro_destino,proceso);
    uint32_t valor_reg_origen = obtenerValorActualRegistro(id_registro_origen,proceso);
    pthread_mutex_lock(&mutex_proceso_actual);
    switch(id_registro_destino){
        case PC:
        {
           proceso->registros_cpu.PC = valor_reg_destino + valor_reg_origen;
            break;
        }
        case AX:
        {
           proceso->registros_cpu.AX = valor_reg_destino + valor_reg_origen;
            break;
        }
        case BX:
        {
           proceso->registros_cpu.BX = valor_reg_destino + valor_reg_origen;
            break;
        }
        case CX:
        {
           proceso->registros_cpu.CX = valor_reg_destino + valor_reg_origen;
            break;
        }
        case DX:
        {
           proceso->registros_cpu.DX = valor_reg_destino + valor_reg_origen;
            break;
        }
        case EX:
        {
           proceso->registros_cpu.EX = valor_reg_destino + valor_reg_origen;
            break;
        }
        case FX:
        {
           proceso->registros_cpu.FX = valor_reg_destino + valor_reg_origen;
            break;
        }
        case GX:
        {
           proceso->registros_cpu.GX = valor_reg_destino + valor_reg_origen;
            break;
        }
        case HX:
        {
           proceso->registros_cpu.HX = valor_reg_destino + valor_reg_origen;
            break;
        }
        
        default:
        log_trace(logger_cpu, "El registro no existe");
    }
    pthread_mutex_unlock(&mutex_proceso_actual);


    
}

void sub(char* registro_destino, char* registro_origen, t_proceso* proceso){
    registros id_registro_destino = identificarRegistro(registro_destino);
    registros id_registro_origen = identificarRegistro(registro_origen);

    uint32_t valor_reg_destino = obtenerValorActualRegistro(id_registro_destino,proceso);
    uint32_t valor_reg_origen = obtenerValorActualRegistro(id_registro_origen,proceso);
    pthread_mutex_lock(&mutex_proceso_actual);
    switch(id_registro_destino){
        case PC:
        {
           proceso->registros_cpu.PC = valor_reg_destino - valor_reg_origen;
            break;
        }
        case AX:
        {
           proceso->registros_cpu.AX = valor_reg_destino - valor_reg_origen;
            break;
        }
        case BX:
        {
           proceso->registros_cpu.BX = valor_reg_destino - valor_reg_origen;
            break;
        }
        case CX:
        {
           proceso->registros_cpu.CX = valor_reg_destino - valor_reg_origen;
            break;
        }
        case DX:
        {
           proceso->registros_cpu.DX = valor_reg_destino - valor_reg_origen;
            break;
        }
        case EX:
        {
           proceso->registros_cpu.EX = valor_reg_destino - valor_reg_origen;
            break;
        }
        case FX:
        {
           proceso->registros_cpu.FX = valor_reg_destino - valor_reg_origen;
            break;
        }
        case HX:
        {
           proceso->registros_cpu.HX = valor_reg_destino - valor_reg_origen;
            break;
        }
        case GX:
        {
           proceso->registros_cpu.GX = valor_reg_destino - valor_reg_origen;
            break;
        }        
        default:
        log_trace(logger_cpu, "El registro no existe");
    }
    pthread_mutex_unlock(&mutex_proceso_actual);
  
}


void jnz(char* registro, char* inst_char, t_proceso* proceso){
    registros id_registro = identificarRegistro(registro);
    uint32_t valor_registro = obtenerValorActualRegistro(id_registro,proceso);
    int inst =  atoi(inst_char)  ;//string_a_uint32(inst_char);  
    if(valor_registro != 0){
        pthread_mutex_lock(&mutex_proceso_actual);
        log_trace(logger_cpu, "valor solcitado JNZ  %d", inst);
        proceso->registros_cpu.PC = inst;
        log_trace(logger_cpu, "valor nuevo  %d", proceso->registros_cpu.PC);
        pthread_mutex_unlock(&mutex_proceso_actual);
    }else{
        proceso->registros_cpu.PC += 1; // continuo en la siguiente instrucción
    }

}

void loguear(char* registro){
    registros id_registro = identificarRegistro(registro);
    uint32_t valor_reg = obtenerValorActualRegistro(id_registro,proceso_actual);
    log_trace(logger_cpu, "valor registro %s: %d",registro, valor_reg);
}

void limpiarCadena(char* cadena) {
    char* token;
    char delimitadores[] = " \n\t"; // Espacio, salto de línea y tabulador
    char cadenaLimpia[100] = ""; // Asegúrate de que esta cadena sea lo suficientemente grande

    // Obtener el primer token
    token = strtok(cadena, delimitadores);
    
    // Iterar sobre los siguientes tokens
    while (token != NULL) {
        strcat(cadenaLimpia, token);
        token = strtok(NULL, delimitadores);
    }

    // Copiar la cadena limpia de vuelta a la cadena original
    strcpy(cadena, cadenaLimpia);
}


registros identificarRegistro(char* registro){
    printf("ENTRO A IDENTIFICAR_REGISTRO: %s\n",registro); 
    log_trace(logger_cpu, "Identificar Registro: %s", registro);
    limpiarCadena(registro);
    if(strcmp(registro,"PC") == 0){
        
        return PC;
    }
    else if(strcmp(registro,"AX") == 0){
         printf("Entro AX \n"); 
        return AX;
    }
    else if(strcmp(registro,"BX") == 0){
        return BX;
    }
    else if(strcmp(registro,"CX") == 0){
        return CX;
    }
    else if(strcmp(registro,"DX") == 0){
        return DX;
    }
    else if(strcmp(registro,"EX") == 0){
        return EX;
    }
    else if(strcmp(registro,"FX") == 0){
        return FX;
    }
    else if(strcmp(registro,"GX") == 0){
        return GX;
    }
    else if(strcmp(registro,"HX") == 0){
        return HX;
    }
    else if(strcmp(registro,"base") == 0){
        return base;
    }
    else if(strcmp(registro,"limite") == 0){
        return limite;
    }
    else{
        return REG_NO_ENC;
    }
}

uint32_t obtenerValorActualRegistro(registros id_registro, t_proceso* proceso){
    switch(id_registro){
        case PC:
        {
           return proceso->registros_cpu.PC;
            break;
        }
        case AX:
        {
           return proceso->registros_cpu.AX;
            break;
        }
        case BX:
        {
           return proceso->registros_cpu.BX;
            break;
        }
        case CX:
        {
           return proceso->registros_cpu.CX;
            break;
        }
        case DX:
        {
           return proceso->registros_cpu.DX;
            break;
        }
        case EX:
        {
           return proceso->registros_cpu.EX;
            break;
        }
        case FX:
        {
           return proceso->registros_cpu.FX;
            break;
        }
        case GX:
        {
           return proceso->registros_cpu.GX;
            break;
        }
        case HX:
        {
           return proceso->registros_cpu.HX;
            break;
        }
        case base:
        {
           return proceso->registros_cpu.base;
            break;
        }
        case limite:
        {
           return proceso->registros_cpu.limite;
            break;
        }
      
        default:
        log_trace(logger_cpu, "El registro no existe");
    }
}




uint32_t mmu(uint32_t direccion_logica, t_proceso* proceso, int conexion, int conexion_kernel_dispatch) {
    uint32_t direccion_fisica_resultado;
    uint32_t desplazamiento = direccion_logica;

    log_trace(logger_cpu, "MMU: Inicio - Dirección Lógica: %u, Base: %u, Límite: %u",
             direccion_logica, proceso->registros_cpu.base, proceso->registros_cpu.limite);

    // Validación de límites de partición
    if (proceso->registros_cpu.base + desplazamiento+3 <= proceso->registros_cpu.limite) { // agrego 3 bytes porque cuento la posicion como el byte 1 de los 4 byte que tengo que grabar
        direccion_fisica_resultado = proceso->registros_cpu.base + desplazamiento;
        log_trace(logger_cpu, "MMU: Dirección Física válida - Dirección Física: %u", direccion_fisica_resultado);
        return direccion_fisica_resultado;
    } else {
        // SEG_FAULT
        log_error(logger_cpu, "MMU: SEG_FAULT - Dirección Lógica: %u, Dirección Física Calculada: %u, Límite: %u",
                  direccion_logica, proceso->registros_cpu.base + desplazamiento, proceso->registros_cpu.limite);
        flag_sementation_fault = true;
        enviar_contexto_a_memoria(proceso, conexion);
        sem_wait(&semaforo_sincro_contexto_syscall);
        enviar_segfault_a_kernel(proceso, conexion_kernel_dispatch);
    }
}




void read_mem(char* registro_datos, char* registro_direccion, t_proceso* proceso, int conexion){
    // Lee el valor de memoria correspondiente a la Dirección Lógica que se encuentra en el 
    //Registro Dirección y lo almacena en el Registro Datos
    registros id_registro_direccion = identificarRegistro(registro_direccion);
    
    //uint32_t valor_registro_direccion = malloc(sizeof(uint32_t));
    uint32_t valor_registro_direccion = obtenerValorActualRegistro(id_registro_direccion,proceso);

    uint32_t dir_fisica_result;

    log_trace(logger_cpu, "READ_MEM: id registro Dirección=%u", id_registro_direccion);
    log_trace(logger_cpu, "READ_MEM: valor registro Dirección=%u", valor_registro_direccion);
    log_trace(logger_cpu, "READ_MEM: valor registro datos=%s", registro_datos);

    dir_fisica_result = mmu(valor_registro_direccion,proceso,conexion, conexion_kernel_dispatch);

    registros id_registro_datos = identificarRegistro(registro_datos);
    

    pedir_valor_a_memoria(dir_fisica_result,proceso->pid,proceso->tid,conexion);
    int valor_sem;
    sem_getvalue(&sem_valor_registro_recibido, &valor_sem);
    printf("valor sem_valor_registro_recibido:%d\n", valor_sem);
    sem_wait(&sem_valor_registro_recibido);
    printf("paso sem_valor_registro_recibido\n");

    log_info(logger_cpu, "TID: %u - Acción: LEER - Dirección Física: %u - Valor: %d", proceso_actual->tid,dir_fisica_result,valor_registro_obtenido); //LOG OBLIGATORIO
    //sem_wait(&sem_esperando_read_write_mem); //Revisar aca
    registros registro_elegido = identificarRegistro(registro_datos);
switch(registro_elegido){
        case PC:
        {
          proceso->registros_cpu.PC = valor_registro_obtenido;
            break;
        }
        case AX:
        {
          proceso->registros_cpu.AX = valor_registro_obtenido;
            break;
        }
        case BX:
        {
          proceso->registros_cpu.BX = valor_registro_obtenido;
            break;
        }
        case CX:
        {
          proceso->registros_cpu.CX = valor_registro_obtenido;
            break;
        }
        case DX:
        {
          proceso->registros_cpu.DX = valor_registro_obtenido;
            break;
        }
        case EX:
        {
          proceso->registros_cpu.EX = valor_registro_obtenido;
            break;
        }
        case FX: 
        {
          proceso->registros_cpu.FX = valor_registro_obtenido;
            break;
        }
        case GX:
        {
          proceso->registros_cpu.GX = valor_registro_obtenido;
            break;
        }
        case HX:
        {
          proceso->registros_cpu.HX = valor_registro_obtenido;
            break;
        }
        case base:
        {
          proceso->registros_cpu.base = valor_registro_obtenido;
            break;
        }
        case limite:
        {
          proceso->registros_cpu.limite = valor_registro_obtenido;
            break;
        }
        
        default:
        log_trace(logger_cpu, "El registro no existe");
    }
    //set(registro_datos,valor_registro_obtenido,proceso);

}

//write_mem(inst->param1,inst->param2,proceso,conexion);
void write_mem(char* registro_direccion, char* registro_datos, t_proceso* proceso, int conexion){
    // Lee el valor del Registro Datos y lo escribe en la dirección física de
    // memoria obtenida a partir de la Dirección Lógica almacenada en el Registro Dirección.
    printf("registro: %s\n",registro_datos);
    registros id_registro_datos = identificarRegistro(registro_datos);
    printf("registro: %d\n",id_registro_datos);
    uint32_t valor_registro_datos = obtenerValorActualRegistro(id_registro_datos,proceso);
    printf("manda %d a guardar en mem\n",valor_registro_datos);
    registros id_registro_direccion = identificarRegistro(registro_direccion);
    uint32_t valor_registro_direccion = obtenerValorActualRegistro(id_registro_direccion,proceso);

    log_trace(logger_cpu, "WRITE_MEM: Registro Dirección=%s, Registro Datos=%s", registro_direccion, registro_datos);
    log_trace(logger_cpu, "WRITE_MEM: Valor Dirección=%u, Valor Datos=%u", valor_registro_direccion, valor_registro_datos);


    uint32_t dir_fisica_result = mmu(valor_registro_direccion,proceso,conexion, conexion_kernel_dispatch);


    enviar_valor_a_memoria(dir_fisica_result,proceso->pid,proceso->tid,valor_registro_datos,conexion);
    
    log_info(logger_cpu, "TID: %u - Acción: ESCRIBIR - Dirección Física: %u - Valor: %u", proceso_actual->tid,dir_fisica_result,valor_registro_datos); //LOG OBLIGATORIO

    sem_wait(&sem_esperando_read_write_mem);

}



void pedir_valor_a_memoria(uint32_t dir_fisica, uint32_t pid, uint32_t tid, int conexion){
        printf("entro a pedir_valor_a_memoria\n");
        t_paquete* paquete_pedido_valor_memoria;
        paquete_pedido_valor_memoria = crear_paquete(READ_MEMORIA); 
        agregar_a_paquete(paquete_pedido_valor_memoria,  &pid,  sizeof(uint32_t)); 
        agregar_a_paquete(paquete_pedido_valor_memoria,  &tid,  sizeof(uint32_t));      
        agregar_a_paquete(paquete_pedido_valor_memoria,  &dir_fisica,  sizeof(uint32_t));            
        enviar_paquete(paquete_pedido_valor_memoria, conexion); 
        eliminar_paquete(paquete_pedido_valor_memoria);

}

void enviar_valor_a_memoria(uint32_t dir_fisica, uint32_t pid, uint32_t tid, uint32_t valor, int conexion){
        printf("entro a enviar_valor_a_memoria\n");
        t_paquete* paquete_pedido_valor_memoria;
        paquete_pedido_valor_memoria = crear_paquete(WRITE_MEMORIA); 
        log_trace(logger_cpu, "direccion fisica a enviar: %d valor: %d \n", dir_fisica, valor);
        agregar_a_paquete(paquete_pedido_valor_memoria,  &pid,  sizeof(uint32_t)); 
        agregar_a_paquete(paquete_pedido_valor_memoria,  &tid,  sizeof(uint32_t));      
        agregar_a_paquete(paquete_pedido_valor_memoria,  &dir_fisica,  sizeof(uint32_t));
        agregar_a_paquete(paquete_pedido_valor_memoria,  &valor,  sizeof(uint32_t));    
        enviar_paquete(paquete_pedido_valor_memoria, conexion); 
        eliminar_paquete(paquete_pedido_valor_memoria);

}

//////////////////////////////////////// SYSCALLS //////////////////////////////////////////


void enviar_dump_memory_a_kernel(int socket_dispatch){
    printf("entro a enviar_dump_memory_a_kernel\n");
    t_paquete* paquete_dump_memory;   
    paquete_dump_memory = crear_paquete(PEDIDO_MEMORY_DUMP);     
    
    agregar_a_paquete(paquete_dump_memory, &proceso_actual->pid,  sizeof(uint32_t));
    agregar_a_paquete(paquete_dump_memory, &proceso_actual->tid,  sizeof(uint32_t));
    enviar_paquete(paquete_dump_memory, socket_dispatch); 
    eliminar_paquete(paquete_dump_memory); 
    //proceso_actual = NULL;   
}


void enviar_io_a_kernel(char* tiempo ,int socket_dispatch){
    printf("entro a enviar_io_a_kernel\n");
    t_paquete* paquete_io;
    char *endptr;   
    paquete_io = crear_paquete(IO_EJECUTAR);     
    uint32_t tiempo_num = (uint32_t)strtoul(tiempo, &endptr, 10);// Convertir la cadena a uint32_t
    
    agregar_a_paquete(paquete_io, &proceso_actual->pid,  sizeof(uint32_t));
    agregar_a_paquete(paquete_io, &proceso_actual->tid,  sizeof(uint32_t));
    agregar_a_paquete(paquete_io, &tiempo_num,  sizeof(uint32_t));
    enviar_paquete(paquete_io, socket_dispatch); 
    eliminar_paquete(paquete_io);  
   // proceso_actual = NULL;     
}

void enviar_process_create_a_kernel(char* nombre_pseudocodigo, char* tamanio_proceso, char* prioridad_hilo, int socket_dispatch){
    printf("entro a enviar_process_create_a_kernel\n");
    t_paquete* paquete_create_process;
    char *endptr;
    paquete_create_process = crear_paquete(PROCESO_CREAR); //AGREGAR LA OPERACION CORESPONDENTIE
    int tamanio_nombre_pseudocodigo = string_length(nombre_pseudocodigo)+1;
  
    //agregar_a_paquete(paquete_create_process, &tamanio_nombre_pseudocodigo,  sizeof(uint32_t));
    agregar_a_paquete(paquete_create_process, nombre_pseudocodigo, tamanio_nombre_pseudocodigo);
    uint32_t tamanio_proceso_num = (uint32_t)strtoul(tamanio_proceso, &endptr, 10);// Convertir la cadena a uint32_t
    agregar_a_paquete(paquete_create_process, &tamanio_proceso_num,  sizeof(uint32_t));
    uint32_t prioridad_hilo_num = (uint32_t)strtoul(prioridad_hilo, &endptr, 10);// Convertir la cadena a uint32_t
    agregar_a_paquete(paquete_create_process, &prioridad_hilo_num,  sizeof(uint32_t));
    enviar_paquete(paquete_create_process, socket_dispatch); 
    eliminar_paquete(paquete_create_process);
   // proceso_actual = NULL;

}

void enviar_thread_create_a_kernel(char* nombre_pseudocodigo, char* prioridad_hilo, int socket_dispatch){
    log_trace(logger_cpu,"entro a enviar_thread_create_a_kernel");
    t_paquete* paquete_create_thread;
    char *endptr;
    paquete_create_thread = crear_paquete(HILO_CREAR); //AGREGAR LA OPERACION CORESPONDENTIE
    int tamanio_nombre_pseudocodigo = string_length(nombre_pseudocodigo)+1;
  
    //agregar_a_paquete(paquete_create_thread, &tamanio_nombre_pseudocodigo,  sizeof(uint32_t));
    agregar_a_paquete(paquete_create_thread, nombre_pseudocodigo, tamanio_nombre_pseudocodigo);     
    uint32_t prioridad_hilo_num = (uint32_t)strtoul(prioridad_hilo, &endptr, 10);// Convertir la cadena a uint32_t
    agregar_a_paquete(paquete_create_thread, &prioridad_hilo_num,  sizeof(uint32_t));
    enviar_paquete(paquete_create_thread, socket_dispatch); 
    eliminar_paquete(paquete_create_thread);
    //proceso_actual = NULL;
}


void enviar_thread_join_a_kernel(char* tid ,int socket_dispatch){
    printf("entro a enviar_thread_join_a_kernel\n");
    t_paquete* paquete_thread_join;
    char *endptr;
    paquete_thread_join = crear_paquete(HILO_JUNTAR); 
    uint32_t tid_numero = (uint32_t)strtoul(tid, &endptr, 10);
       
    agregar_a_paquete(paquete_thread_join, &tid_numero,  sizeof(uint32_t));
    enviar_paquete(paquete_thread_join, socket_dispatch); 
    eliminar_paquete(paquete_thread_join); 
    //proceso_actual = NULL;   
}


void enviar_thread_cancel_a_kernel(char* tid ,int socket_dispatch){
    printf("entro a enviar_thread_cancel_a_kernel\n");
    t_paquete* paquete_thread_cancel;
    char *endptr;
    paquete_thread_cancel = crear_paquete(HILO_CANCELAR); 
    uint32_t tid_numero = (uint32_t)strtoul(tid, &endptr, 10);
       
    agregar_a_paquete(paquete_thread_cancel, &tid_numero,  sizeof(uint32_t));
    enviar_paquete(paquete_thread_cancel, socket_dispatch); 
    eliminar_paquete(paquete_thread_cancel);  
    //proceso_actual = NULL;    
}


void enviar_mutex_create_a_kernel(char* recurso, int conexion_kernel){
    printf("entro a enviar mutex create a kernell\n");        
    t_paquete* paquete_mutex_create_kernel;   
    paquete_mutex_create_kernel = crear_paquete(MUTEX_CREAR); 
    int tamanio_recurso = strlen(recurso)+1;

    agregar_a_paquete(paquete_mutex_create_kernel,  &proceso_actual->pid,  sizeof(uint32_t));  
   // agregar_a_paquete(paquete_mutex_create_kernel,  &tamanio_recurso,  sizeof(uint32_t));       
    agregar_a_paquete(paquete_mutex_create_kernel,  recurso,  tamanio_recurso);            
    enviar_paquete(paquete_mutex_create_kernel, conexion_kernel); 
    eliminar_paquete(paquete_mutex_create_kernel);
    //proceso_actual = NULL;

}


void enviar_mutex_lock_a_kernel(char* recurso, int conexion_kernel){
    printf("entro a enviar_mutex_lock_a_kernel\n");        
    t_paquete* paquete_lock_kernel;   
    paquete_lock_kernel = crear_paquete(MUTEX_BLOQUEAR); 
    int tamanio_recurso = strlen(recurso)+1;

    //agregar_a_paquete(paquete_lock_kernel,  &proceso_actual->pid,  sizeof(uint32_t));  
    //agregar_a_paquete(paquete_lock_kernel,  &tamanio_recurso,  sizeof(uint32_t));       
    agregar_a_paquete(paquete_lock_kernel,  recurso,  tamanio_recurso);            
    enviar_paquete(paquete_lock_kernel, conexion_kernel); 
    eliminar_paquete(paquete_lock_kernel);
   // proceso_actual = NULL;

}

void enviar_mutex_unlock_a_kernel(char* recurso, int conexion_kernel){
    printf("entro a enviar_mutex_unlock_a_kernel\n");
    t_paquete* paquete_unlock_kernel;   
    paquete_unlock_kernel = crear_paquete(MUTEX_DESBLOQUEAR); 
    int tamanio_recurso = strlen(recurso)+1;

    agregar_a_paquete(paquete_unlock_kernel,  &proceso_actual->pid,  sizeof(uint32_t));   
    agregar_a_paquete(paquete_unlock_kernel,  &tamanio_recurso,  sizeof(uint32_t));       
    agregar_a_paquete(paquete_unlock_kernel,  recurso,  tamanio_recurso);      
    enviar_paquete(paquete_unlock_kernel, conexion_kernel); 
    eliminar_paquete(paquete_unlock_kernel);
    //proceso_actual = NULL;
}


void enviar_process_exit_a_kernel(int conexion_kernel){
    t_paquete* paquete_process_exit_kernel;   
    paquete_process_exit_kernel = crear_paquete(PROCESO_SALIR); 
    
    agregar_a_paquete(paquete_process_exit_kernel,  &proceso_actual->pid,  sizeof(uint32_t));   
    enviar_paquete(paquete_process_exit_kernel, conexion_kernel); 
    eliminar_paquete(paquete_process_exit_kernel);    
   // proceso_actual = NULL;
}


void enviar_thread_exit_a_kernel(int conexion_kernel){
    t_paquete* paquete_thread_exit_kernel;   
    paquete_thread_exit_kernel = crear_paquete(HILO_SALIR);  
    agregar_a_paquete(paquete_thread_exit_kernel,  &proceso_actual->pid,  sizeof(uint32_t)); 
    agregar_a_paquete(paquete_thread_exit_kernel,  &proceso_actual->tid,  sizeof(uint32_t));     
    enviar_paquete(paquete_thread_exit_kernel, conexion_kernel); 
    eliminar_paquete(paquete_thread_exit_kernel);  
    //proceso_actual = NULL;    
}


////////////////////////////////  UTILS //////////////////////////////////////////

void imprimir_contenido_paquete(t_paquete* paquete);
void imprimir_contenido_paquete(t_paquete* paquete) {
    printf("Codigo de operacion: %d\n", paquete->codigo_operacion);
    printf("Tamaño del buffer: %d\n", paquete->buffer->size);
    printf("Contenido del buffer:\n");

    uint8_t* stream = (uint8_t*) paquete->buffer->stream;
    for (int i = 0; i < paquete->buffer->size; i++) {
        printf("%02X ", stream[i]);
    }
    printf("\n");
}

bool es_syscall(tipo_instruccion tipo_instru){
    return tipo_instru==THREAD_CANCEL || 
           tipo_instru==THREAD_CREATE ||
           tipo_instru==THREAD_EXIT ||
           tipo_instru==PROCESS_CREATE ||
        tipo_instru==PROCESS_EXIT||
        tipo_instru==MUTEX_CREATE||
        tipo_instru==MUTEX_LOCK||
        tipo_instru==MUTEX_UNLOCK ||
        tipo_instru==IO ||
        tipo_instru==DUMP_MEMORY ||
        tipo_instru==THREAD_JOIN;
}
bool ciclo_de_instrucciones(int *conexion_mer, t_proceso *proceso, int *socket_dispatch, int *socket_interrupt)
{  
    int conexion_mem = *conexion_mer;
    int dispatch = *socket_dispatch;
    int interrupt = *socket_interrupt;
   
    //free(conexion_mer);
    //free(socket_dispatch);
    //free(socket_interrupt);
    
    log_trace(logger_cpu, "Entro al ciclo");

    //instr_t *inst = malloc(sizeof(instr_t));
    log_trace(logger_cpu, "Voy a entrar a fetch");
    instr_t *inst = fetch(conexion_mem,proceso); 
    tipo_instruccion tipo_inst;
    log_trace(logger_cpu, "Voy a entrar a decode");
    
    tipo_inst= decode(inst, conexion_mem);
    log_trace(logger_cpu, "Voy a entrar a execute");

    execute(inst, tipo_inst, proceso, conexion_mem, dispatch, interrupt);
    if (!es_syscall(tipo_inst) && tipo_inst != JNZ) 
    {
        proceso->registros_cpu.PC += 1;
    }
    
    if(es_syscall(tipo_inst) || flag_sementation_fault ){
        if (flag_sementation_fault){
            flag_sementation_fault = false;
        }
        sem_wait(&semaforo_respuesta_syscall);// el post se hace con respuestas del puerto de interrupt
        if(respuesta_syscall==REPLANIFICACION){  // ISSUE: 4396
        log_trace(logger_cpu, "EL PROCESO ACTUAL desalojado por syscall, esperando otro...");
       //free(proceso_actual); este free pone en null tambien al proceso pasado por parametro a esta funcion
        pthread_mutex_lock(&mutex_proceso_actual);
        if (proceso_actual != NULL) {
                free(proceso_actual);
                 proceso_actual = NULL;
                }
        pthread_mutex_unlock(&mutex_proceso_actual);
        pthread_mutex_lock(&mutex_interrupcion_kernel);
        interrupcion_kernel = false;
        respuesta_syscall=-1;
        pthread_mutex_unlock(&mutex_interrupcion_kernel);
        return true;
        //che kernel, ya termine de 
        }
    }
    

    log_trace(logger_cpu, "Voy a entrar a check_interrupt");
   // check_interrupt(dispatch);
    pthread_mutex_lock(&mutex_interrupcion_kernel);
    if(interrupcion_kernel && proceso_actual != NULL){
        log_trace(logger_cpu,"ENTRO EN IF DEL  CHECK INTERRUPT\n");
        enviar_contexto_a_memoria(proceso_actual,socket_memoria);
        sem_wait(&semaforo_sincro_contexto_syscall);
        enviar_fin_quantum_a_kernel(proceso_actual,dispatch );
        interrupcion_kernel = false;
        respuesta_syscall=-1;
        pthread_mutex_unlock(&mutex_interrupcion_kernel);
        return true;
    }
    pthread_mutex_unlock(&mutex_interrupcion_kernel);

   // free(inst->param1);
   // free(inst->param2);
   // free(inst->param3);
   // free(inst->param4);
   // free(inst->param5);
    //free(inst);
    log_trace(logger_cpu, "Termino ciclo de instrucciones");   
    return false;//continuara con el siguiente ciclo
}

tipo_instruccion str_to_tipo_instruccion(const char *str) {
    char *mutable_str = strdup(str);  // Creamos una copia mutable
    trim_newline(mutable_str);

    tipo_instruccion instruccion_a_devolver = -1;
    if (strcmp(mutable_str, "SET") == 0) instruccion_a_devolver = SET;
    else if (strcmp(mutable_str, "READ_MEM") == 0) instruccion_a_devolver = READ_MEM;
    else if (strcmp(mutable_str, "WRITE_MEM") == 0) instruccion_a_devolver = WRITE_MEM;
    else if (strcmp(mutable_str, "SUM") == 0) instruccion_a_devolver = SUM;
    else if (strcmp(mutable_str, "SUB") == 0) instruccion_a_devolver = SUB;
    else if (strcmp(mutable_str, "JNZ") == 0) instruccion_a_devolver = JNZ;
    else if (strcmp(mutable_str, "LOG") == 0) instruccion_a_devolver = LOG;
    else if (strcmp(mutable_str, "DUMP_MEMORY") == 0) instruccion_a_devolver = DUMP_MEMORY;
    else if (strcmp(mutable_str, "IO") == 0) instruccion_a_devolver = IO;
    else if (strcmp(mutable_str, "PROCESS_CREATE") == 0) instruccion_a_devolver = PROCESS_CREATE;
    else if (strcmp(mutable_str, "THREAD_CREATE") == 0) instruccion_a_devolver = THREAD_CREATE;
    else if (strcmp(mutable_str, "THREAD_JOIN") == 0) instruccion_a_devolver = THREAD_JOIN;
    else if (strcmp(mutable_str, "THREAD_CANCEL") == 0) instruccion_a_devolver = THREAD_CANCEL;
    else if (strcmp(mutable_str, "MUTEX_CREATE") == 0) instruccion_a_devolver = MUTEX_CREATE;
    else if (strcmp(mutable_str, "MUTEX_LOCK") == 0) instruccion_a_devolver = MUTEX_LOCK;
    else if (strcmp(mutable_str, "MUTEX_UNLOCK") == 0) instruccion_a_devolver = MUTEX_UNLOCK;
    else if (strcmp(mutable_str, "THREAD_EXIT") == 0) instruccion_a_devolver = THREAD_EXIT;
    else if (strcmp(mutable_str, "PROCESS_EXIT") == 0) instruccion_a_devolver = PROCESS_EXIT;
    else log_trace(logger_cpu, "Instrucción desconocida: %s", str);

    log_trace(logger_cpu, "Código de instrucción devuelto: %d", instruccion_a_devolver);

    free(mutable_str);
    return instruccion_a_devolver;
}


void trim_newline(char *str) {
    char *end = str + strlen(str) - 1;
    if (*end == '\n') {
        *end = '\0';  // Reemplaza el '\n' con un terminador nulo
    }
}

void enviar_contexto_a_memoria(t_proceso* proceso, int conexion){
  
    t_paquete* paquete_devolucion_contexto;

    paquete_devolucion_contexto = crear_paquete(DEVOLUCION_CONTEXTO); 
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->pid,  sizeof(uint32_t));         
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->tid,  sizeof(uint32_t));        
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->registros_cpu.PC, sizeof(uint32_t));
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->registros_cpu.AX, sizeof(uint32_t)); 
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->registros_cpu.BX, sizeof(uint32_t)); 
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->registros_cpu.CX, sizeof(uint32_t)); 
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->registros_cpu.DX, sizeof(uint32_t)); 
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->registros_cpu.EX, sizeof(uint32_t));
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->registros_cpu.FX, sizeof(uint32_t));
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->registros_cpu.HX, sizeof(uint32_t));
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->registros_cpu.GX, sizeof(uint32_t));
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->registros_cpu.base, sizeof(uint32_t));
    agregar_a_paquete(paquete_devolucion_contexto, &proceso->registros_cpu.limite, sizeof(uint32_t));
    log_trace(logger_cpu,"## PC:%d", proceso->registros_cpu.PC);
    log_trace(logger_cpu,"## AX:%d", proceso->registros_cpu.AX);
    log_trace(logger_cpu,"## BX:%d", proceso->registros_cpu.BX);
    log_trace(logger_cpu,"## CD:%d", proceso->registros_cpu.CX);
    log_trace(logger_cpu,"## DX:%d", proceso->registros_cpu.DX);


    enviar_paquete(paquete_devolucion_contexto, conexion); 
    eliminar_paquete(paquete_devolucion_contexto);
    log_info(logger_cpu,"## TID: %d- Actualizo Contexto Ejecución", proceso->tid); // LOG OBLIGATORIO
 }

 void solicitar_contexto_a_memoria(t_proceso* proceso, int conexion){
    printf("entro a paquete_solicitud_contexto\n");
    t_paquete* paquete_solicitud_contexto;

    paquete_solicitud_contexto = crear_paquete(SOLICITUD_CONTEXTO); 
    log_trace(logger_cpu,"pidiendo contexto a memoria pid:%d, tid:%d",proceso->pid,proceso->tid);
    agregar_a_paquete(paquete_solicitud_contexto, &proceso->pid,  sizeof(uint32_t));         
    agregar_a_paquete(paquete_solicitud_contexto, &proceso->tid,  sizeof(uint32_t));
    enviar_paquete(paquete_solicitud_contexto, conexion); 
    eliminar_paquete(paquete_solicitud_contexto);
 }



void enviar_segfault_a_kernel(t_proceso* proceso,int conexion_kernel_dispatch){
    printf("entro a paquete_solicitud_contexto\n");
    t_paquete* paquete_segfault;

    paquete_segfault = crear_paquete(SEGMENTATION_FAULT); 
    agregar_a_paquete(paquete_segfault, &proceso->pid,  sizeof(uint32_t));         
    agregar_a_paquete(paquete_segfault, &proceso->tid,  sizeof(uint32_t));
    enviar_paquete(paquete_segfault, conexion_kernel_dispatch); 
    eliminar_paquete(paquete_segfault);
}  

uint32_t string_a_uint32(const char* str) {
    // Usa la función strtoul para convertir la cadena a un entero sin signo
    char* endptr; // Para manejar errores de conversión
    unsigned long result = strtoul(str, &endptr, 10); // Convertir de base 10

    // Verificar si hubo un error en la conversión
    if (*endptr != '\0') {
        // Manejo de error: la cadena no era un número válido
        return 0; // O manejar de otra manera según sea necesario
    }

   
    if (result > UINT32_MAX) {
      
        return UINT32_MAX; // O manejar de otra manera según sea necesario
    }

    return (uint32_t)result; // Retornar el resultado como uint32_t
}
