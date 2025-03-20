#include "../include/main.h"
int main(int argc, char* argv[]) {
    char* path_config = argv[1];
    iniciar_modulo(path_config);
    generar_conexiones_a_cpu();
        //    int conexion=conectar_a_memoria();
        //      int soli_hand=HANDSHAKE;
        // send(conexion,&soli_hand, sizeof(uint32_t), MSG_WAITALL);
        // log_trace(logger_kernel,"ENIVADO DESDE MAIN");
    
    int tamanio=atoi(argv[3]);
    process_create(argv[2],tamanio,HILO_MAIN);//PROCESO KERNEL INICIAL
    log_trace(logger_kernel,"main envio a crear procesos y espera a que se termine el planificador largo plazo(while 1)");
    //  mostrar_pcb(list_get(lista_new,0),logger_kernel);
    //  mostrar_pcb(list_get(lista_new,1),logger_kernel);    
    pthread_join(hilos->hilo_planif_largo_plazo, NULL);
    return 0;

}
