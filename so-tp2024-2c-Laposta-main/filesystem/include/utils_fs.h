
#ifndef UTILS_FS_H_
#define UTILS_FS_H_
#include <utils/utils.h>

typedef struct {
    char* nombre_archivo;
    int tamanio_archivo;
    char *contenido;
}t_dumped;

t_dumped* dumped_deserializar(t_list* lista);
void liberar_t_dumped(t_dumped* dumped);

#endif /* UTILS_FS_H_ */