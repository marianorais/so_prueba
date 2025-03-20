#include "../include/main.h"

int main(int argc, char* argv[]) {

    char* path_config = argv[1];              //para correr por consola
    //char* path_config = "./memoria.config";   //para correr por vsc(debug)

    char* ip_memoria = argv[2];                //para correr por consola
    
    //-------------------Configuraciones---------------------------
    if (!init(path_config) || !cargar_configuracion(path_config)) {

        cerrar_programa();
        return EXIT_FAILURE;
    }

    //-------------------Variables---------------------------
    if (!inicializar_memoria()) {
        cerrar_programa();
        return EXIT_FAILURE;
    }
    log_trace(logger_memoria, "Se inicio correctamente la Memoria");

    //-------------------Servidores------------------------
    iniciar_servidores(ip_memoria);
    log_trace(logger_memoria, "Se inicio correctamente los servidores");

    //void cerrar_programa();
    return 0;
}
