#include "../include/main.h"

char *ip_fs;
char *path_config;
pthread_mutex_t mtx_file_system;

int main(char argc, char *argv[]) {

    path_config = argv[1];
    ip_fs = argv[2];
    printf("iniciando...\n");

    pthread_mutex_init(&mtx_file_system, NULL);

    if (!init(path_config)) {
        cerrar_programa();
        printf("No se pudo inicializar entrada salida");
        return EXIT_FAILURE;
    }

    pthread_t fs;
    pthread_create(&fs, NULL, (void *)iniciar_fs, NULL);
   
    pthread_t servidor_fs;
    pthread_create(&servidor_fs, NULL, (void *)crear_servidor_fs, ip_fs);
    pthread_join (servidor_fs,NULL);
    pthread_join (fs,NULL);
   
    
    



    cerrar_programa();
    return 0;
}