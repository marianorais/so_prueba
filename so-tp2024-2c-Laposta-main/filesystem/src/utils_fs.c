#include"../include/utils_fs.h"


t_dumped* dumped_deserializar(t_list* lista) {
    t_dumped* dumped = malloc(sizeof(t_dumped));
     
    dumped->nombre_archivo = list_get(lista, 0);     
    dumped->tamanio_archivo = *(uint32_t*)list_get(lista, 1);
    dumped->contenido = (char*)list_get(lista, 2);
    return dumped;

}

void liberar_t_dumped(t_dumped* dumped) {
    if (dumped == NULL) {
        return; 
    }
   
    if (dumped->nombre_archivo != NULL) {
        free(dumped->nombre_archivo);
    }
    if (dumped->contenido != NULL) {
        free(dumped->contenido);
    }
    free(dumped);
}