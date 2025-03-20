#include "../include/init_cpu.h"

t_log *logger_cpu; // Definición de la variable global
t_config_cpu *cfg_cpu;
t_config *file_cfg_cpu;
int base_particion;

int checkProperties(char *path_config) {
    // config valida
    t_config *config = config_create(path_config);
    if (config == NULL) {
        log_error(logger_cpu, "Ocurrió un error al intentar abrir el archivo config");
        return false;
    }

    char *properties[] = {
            "IP_MEMORIA",
            "PUERTO_MEMORIA",
            "PUERTO_ESCUCHA_DISPATCH",
            "PUERTO_ESCUCHA_INTERRUPT",
            "LOG_LEVEL",
            NULL
            };

    // Falta alguna propiedad
    if (!config_has_all_properties(config, properties)) {
        log_error(logger_cpu, "Propiedades faltantes en el archivo de configuracion");
        return false;
    }

    config_destroy(config);

    return true;
}

int cargar_configuracion(char *path) {

    file_cfg_cpu = config_create(path);

    cfg_cpu->IP_MEMORIA = strdup(config_get_string_value(file_cfg_cpu, "IP_MEMORIA"));
    log_trace(logger_cpu, "IP_MEMORIA cargado correctamente: %s", cfg_cpu->IP_MEMORIA);

    cfg_cpu->PUERTO_MEMORIA = strdup(config_get_string_value(file_cfg_cpu, "PUERTO_MEMORIA"));
    log_trace(logger_cpu, "PUERTO_MEMORIA cargado correctamente: %s", cfg_cpu->PUERTO_MEMORIA);

    cfg_cpu->PUERTO_ESCUCHA_DISPATCH = strdup(config_get_string_value(file_cfg_cpu, "PUERTO_ESCUCHA_DISPATCH"));
    log_trace(logger_cpu, "PUERTO_ESCUCHA_DISPATCH cargado correctamente: %s", cfg_cpu->PUERTO_ESCUCHA_DISPATCH);

    cfg_cpu->PUERTO_ESCUCHA_INTERRUPT = strdup(config_get_string_value(file_cfg_cpu, "PUERTO_ESCUCHA_INTERRUPT"));
    log_trace(logger_cpu, "PUERTO_ESCUCHA_INTERRUPT cargado correctamente: %s", cfg_cpu->PUERTO_ESCUCHA_INTERRUPT);

    cfg_cpu-> LOG_LEVEL = strdup(config_get_string_value(file_cfg_cpu, "LOG_LEVEL"));
    log_trace(logger_cpu, "LOG LEVEL cargado correctamente: %s", cfg_cpu->LOG_LEVEL);


    log_trace(logger_cpu, "Archivo de configuracion cargado correctamente");
    config_destroy(file_cfg_cpu);
    return true;
}



int init(char *path_config) {
    //inicializo estructura de configuracion
    cfg_cpu = cfg_cpu_start();
    t_config *config_previo = config_create(path_config);

    //logger_cpu = log_create("cpu.log", "CPU", true, LOG_LEVEL_INFO);
    logger_cpu = log_create("cpu.log", "CPU", true, log_level_from_string(config_get_string_value(config_previo, "LOG_LEVEL")));
    if (logger_cpu == NULL) {
        printf("No pude crear el logger");
        return false;
    }

 
    config_destroy(config_previo);
    return checkProperties(path_config);
}




   



void cerrar_programa() {
    //cortar_conexiones();
    //cerrar_servers();  
    //config_destroy(file_cfg_cpu);
    free(cfg_cpu->IP_MEMORIA);
    free(cfg_cpu->PUERTO_ESCUCHA_DISPATCH);
    free(cfg_cpu->PUERTO_ESCUCHA_INTERRUPT);
    free(cfg_cpu);

    log_trace(logger_cpu,"TERMINADA_LA_CONFIG");
    log_trace(logger_cpu, "TERMINANDO_EL_LOG");
    log_destroy(logger_cpu);
}