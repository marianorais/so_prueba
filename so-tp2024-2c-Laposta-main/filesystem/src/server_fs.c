#include"../include/server_fs.h"

char* puerto_escucha;
char * puerto_interrupt;
int fd_mod2 = -1;
int fd_mod3 = -1;


void* crear_servidor_fs(char* ip_file_system){
      

    puerto_escucha = malloc((strlen(cfg_file_system->PUERTO_ESCUCHA) + 1) * sizeof(char));
if (puerto_escucha != NULL) {
    strcpy(puerto_escucha, cfg_file_system->PUERTO_ESCUCHA);
}
else{
    log_info(logger_file_system,"error al asignar memoria a variable del puerto");
}
   
    log_info(logger_file_system, "crea puerto_escucha");

    fd_mod2 = iniciar_servidor(logger_file_system, "SERVER file_system ", ip_file_system,  puerto_escucha);
    log_info(logger_file_system, "inicio servidor");
    if (fd_mod2 == 0) {
        log_error(logger_file_system, "Fallo al crear el servidor, cerrando file_system");
        return EXIT_FAILURE;
    }
    

    while (server_escuchar_fs((uint32_t)fd_mod2, &conexion_memoria));
}

int server_escuchar_fs(int server_socket, int *global_socket) {
    log_info(logger_file_system, "entra a server escuchar");
    int cliente_socket = esperar_cliente(logger_file_system, "FILE_SYSTEM", server_socket);
    log_info(logger_file_system, "cliente conectado socket %d", cliente_socket);
    *global_socket = cliente_socket; 
    
    
    if (cliente_socket != -1) {
        pthread_t atenderProcesoNuevo;
         t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
       
        args->fd = cliente_socket;
        args->server_name = "FILE_SYSTEM";
        
      
        pthread_create(&atenderProcesoNuevo, NULL,(void*)procesar_conexion,(void*)args);
        pthread_detach(atenderProcesoNuevo);
  
        return 1;
    }
    return 0;
}

void procesar_conexion(void *v_args){
     t_procesar_conexion_args *args = (t_procesar_conexion_args *) v_args;
   
    int cliente_socket = args->fd;
    char *server_name = args->server_name;
    free(args);

     op_code cop;
 
    while (cliente_socket != -1) {
   
        if (recv(cliente_socket, &cop, sizeof(uint32_t), MSG_WAITALL) != sizeof(uint32_t)) {
            log_info(logger_file_system, "DISCONNECT!");

            break;
        }
           printf("COP:%d\n",cop);

        switch (cop){
    
            case CREACION_DUMP:
            {
                printf("CREACION_DUMP recibido\n");
                t_list* lista_paquete = recibir_paquete(cliente_socket);
                printf("paquete recibido\n");
                t_dumped* dumped = dumped_deserializar(lista_paquete); 
                printf("paquete deserealizado\n");
                pthread_mutex_lock(&mtx_file_system); // cambiar a clase mutex
                dumpear(dumped, cliente_socket);
                pthread_mutex_unlock(&mtx_file_system);
              
                list_destroy_and_destroy_elements(lista_paquete,free);
                free(dumped);
                break;  
            }
            default:
            {
                printf("Codigo de operacion no identifcado\n");
                break;
            }
            
           
        }   
 
    }
}


