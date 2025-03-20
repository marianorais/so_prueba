#include "../include/init_filesystem.h"

t_log *logger_file_system; // Definición de la variable global
t_config_file_system *cfg_file_system;
t_config *file_cfg_file_system;
int conexion_memoria;

int checkProperties(char *path_config) {
    // config valida
    t_config *config = config_create(path_config);
    if (config == NULL) {
        log_error(logger_file_system, "Ocurrió un error al intentar abrir el archivo config");
        return false;
    }

    char *properties[] = {
            "PUERTO_ESCUCHA",
            "MOUNT_DIR",
            "BLOCK_SIZE",
            "BLOCK_COUNT",
            "RETARDO_ACCESO_BLOQUE",
            "LOG_LEVEL",
            NULL
            };

    // Falta alguna propiedad
    if (!config_has_all_properties(config, properties)) {
        log_error(logger_file_system, "Propiedades faltantes en el archivo de configuracion");
        return false;
    }

    config_destroy(config);

    return true;
}

int cargar_configuracion(char *path) {

    file_cfg_file_system = config_create(path);
    
    logger_file_system = log_create("file_system.log", "file_system", true, log_level_from_string(strdup(config_get_string_value(file_cfg_file_system,"LOG_LEVEL"))));
    
    cfg_file_system->PUERTO_ESCUCHA = strdup(config_get_string_value(file_cfg_file_system, "PUERTO_ESCUCHA"));
    log_info(logger_file_system, "PUERTO_ESCUCHA cargado correctamente: %s", cfg_file_system->PUERTO_ESCUCHA);

    cfg_file_system->MOUNT_DIR = strdup(config_get_string_value(file_cfg_file_system, "MOUNT_DIR"));
    log_info(logger_file_system, "MOUNT_DIR cargado correctamente: %s", cfg_file_system->MOUNT_DIR);

    cfg_file_system->BLOCK_SIZE = config_get_int_value(file_cfg_file_system, "BLOCK_SIZE");
    log_info(logger_file_system, "BLOCK_SIZE cargado correctamente: %d", cfg_file_system->BLOCK_SIZE);

    cfg_file_system->BLOCK_COUNT = config_get_int_value(file_cfg_file_system, "BLOCK_COUNT");
    log_info(logger_file_system, "BLOCK_COUNT cargado correctamente: %d", cfg_file_system->BLOCK_COUNT);

    cfg_file_system->RETARDO_ACCESO_BLOQUE = config_get_int_value(file_cfg_file_system, "RETARDO_ACCESO_BLOQUE");
    log_info(logger_file_system, "RETARDO_ACCESO_BLOQUE cargado correctamente: %d", cfg_file_system->RETARDO_ACCESO_BLOQUE);
    

    cfg_file_system-> LOG_LEVEL = strdup(config_get_string_value(file_cfg_file_system, "LOG_LEVEL"));
    log_info(logger_file_system, "LOG LEVEL cargado correctamente: %s", cfg_file_system->LOG_LEVEL);


    log_info(logger_file_system, "Archivo de configuracion cargado correctamente");
    config_destroy(file_cfg_file_system);
    return true;
}



int init(char *path_config) {
    //inicializo estructura de configuracion
    cfg_file_system = cfg_file_system_start();

    
  
    //inicializo el archivo de configuracion
    cargar_configuracion(path_config);
    if (logger_file_system == NULL) {
        printf("No pude crear el logger");
        return false;
    }
    return checkProperties(path_config);
}




   



void cerrar_programa() {
    //cortar_conexiones();
    //cerrar_servers();  
    //config_destroy(file_cfg_file_system);
   
    free(cfg_file_system);

    log_info(logger_file_system,"TERMINADA_LA_CONFIG");
    log_info(logger_file_system, "TERMINANDO_EL_LOG");
    log_destroy(logger_file_system);
}
