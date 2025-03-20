#include "../include/instrucciones.h"




//Funcion que crea la lista de instrucciones dado un puntero al archivo de un proceso
void leer_instrucciones(char* nombre_archivo, uint32_t proceso_pid, uint32_t hilo_tid) {

	t_miniPCB *miniPCB = busco_proceso_por_PID(proceso_pid);
    t_hilo *hilo_proceso = busco_hilo_por_TID(hilo_tid, miniPCB);

	//Creamos una variable que gurada el path entero: path_instrucciones/nombre
	char* path_total = string_new();
	string_append(&path_total, cfg_memoria->PATH_INSTRUCCIONES);
	string_append(&path_total, "/");
	string_append(&path_total, nombre_archivo);

    //creamos una variable que guarda el archivo
    FILE* archivo = fopen(path_total, "r");		
    log_trace(logger_memoria, "%s", nombre_archivo);

	//Si no se puede abrir el archivo marca error
	if (!archivo){
        log_error(logger_memoria, "Error al abrir el archivo %s.", path_total);
        exit(EXIT_FAILURE);
    }

	char *linea;
	size_t len;
	linea = string_new();
	len = 0;

    //mientras no sea el fin del archivo
	//devuelve la cnatidad de stram leidos
	while(getline(&linea, &len, archivo) != -1) {
		
		if (strcmp(linea, "EXIT") == 0){    //Si la línea leída es "EXIT"

            //Calculamos el tamanio del nombre mas caracter nulo
            int tamanio_del_nombre = strlen(linea) + 1;

            char *linea2 = malloc(tamanio_del_nombre);
            strcpy(linea2, linea);

            //Agregamos la linea a la lista de instrucciones
            list_add(hilo_proceso->lista_de_instrucciones, linea2);
        }
        else{       //Si la línea leída no es "EXIT"

            //Calculamos el tamanio del nombre y luego se elimina el nulo
            int tamanio_del_nombre = strlen(linea);
            linea[tamanio_del_nombre] = '\0';

            char *linea2 = malloc(tamanio_del_nombre);
            strcpy(linea2, linea);

            //Agregamos la linea a la lista de instrucciones
            list_add(hilo_proceso->lista_de_instrucciones, linea2);
        }
	}
    //Agregamos el proceso a la lista de procesos
	list_add(miniPCB->hilos, hilo_proceso);
    pthread_mutex_lock(&mutex_lista_miniPCBs);
    list_add(lista_miniPCBs, miniPCB);
    pthread_mutex_unlock(&mutex_lista_miniPCBs);

    fclose(archivo);
}

//Funcion que asigna a la lista de instrucciones del hilo lo leido del archivo
void leer_instrucciones_particiones_fijas(char* nombre_archivo, t_hilo* hilo){

	

	//Creamos una variable que gurada el path entero: path_instrucciones/nombre
	char* path_total = string_new();
	string_append(&path_total, cfg_memoria->PATH_INSTRUCCIONES);
	string_append(&path_total, "/");
	string_append(&path_total, nombre_archivo);

    //creamos una variable que guarda el archivo
    FILE* archivo = fopen(path_total, "r");		
    log_trace(logger_memoria, "leyendo archivo abierto: %s", nombre_archivo);

	//Si no se puede abrir el archivo marca error
	if (!archivo){
        log_error(logger_memoria, "Error al abrir el archivo %s.", path_total);
        free(path_total);
        exit(EXIT_FAILURE);
    }

	char *linea = NULL;
	size_t len;
	//linea = string_new();
	len = 0;
int contador=0;
    //mientras no sea el fin del archivo
	//devuelve la cnatidad de stram leidos
	while(getline(&linea, &len, archivo) != -1) {
		
		if (strcmp(linea, "EXIT") == 0){    //Si la línea leída es "EXIT"

            //Calculamos el tamanio del nombre mas caracter nulo
            int tamanio_del_nombre = strlen(linea) + 1;

            char *linea2 = malloc(tamanio_del_nombre);
            strcpy(linea2, linea);
            //Agregamos la linea a la lista de instrucciones
            list_add(hilo->lista_de_instrucciones, linea2);
            log_trace(logger_memoria,"instruccion: %s",(char*)list_get(hilo->lista_de_instrucciones,contador));
        }
        else{       //Si la línea leída no es "EXIT"

            //Calculamos el tamanio del nombre y luego se elimina el nulo
            int tamanio_del_nombre = strlen(linea)+1;
            linea[tamanio_del_nombre] = '\0';

            char *linea2 = malloc(tamanio_del_nombre);
            strcpy(linea2, linea);

            //Agregamos la linea a la lista de instrucciones
            list_add(hilo->lista_de_instrucciones, linea2);
            log_trace(logger_memoria,"instruccion: %s",(char*)list_get(hilo->lista_de_instrucciones,contador));

        } 
        contador++;

        free(linea);
        linea = NULL;
	}
     log_trace(logger_memoria,"cantidad de instruc leidas: %d",list_size(hilo->lista_de_instrucciones));

    free(path_total);
    free(linea);
    fclose(archivo);
}



//Funcion que busca una instruccion en base a un pid, tid y pc
char *buscar_instruccion(uint32_t proceso_pid, uint32_t hilo_tid, int program_counter){

    //Buscamo el proceso
	//Recorremos segun el tamaño de la lista de procesos
    for (int i = 0; i < list_size(lista_miniPCBs); i++){

		//Creamos una variable a la que le asignamos elementos de la lista
        t_miniPCB *miniPCB = list_get(lista_miniPCBs, i);

		//Verificamos que sea igual al proceso buscado
        if (miniPCB->pid == proceso_pid){

         log_trace(logger_memoria,"pid encontrado para buscar instruccion: %d",miniPCB->pid);

            //Buscamos el hilo
            //Recorremos segun el tamaño de la lista de hilos
            for (int j = 0; j < list_size(miniPCB->hilos); j++){
                
                t_hilo *hilo_proceso = list_get(miniPCB->hilos, j);
            
                //Verificamos que sea igual al hilo buscado
                if (hilo_proceso->tid == hilo_tid){
                 log_trace(logger_memoria,"tamanio lista_instruciones: %d",list_size(hilo_proceso->lista_de_instrucciones));
                    //Guardamos valor y lo retornamos
                    char *valor = list_get(hilo_proceso->lista_de_instrucciones, program_counter);
                    return valor;
                }

            }
        }
    }

    return NULL;
}

