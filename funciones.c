#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <regex.h>
#include "funciones.h"

regex_t regex; // Expresión regular
typedef struct
{
    char pid[16];
    char info[2048];
} ProcInfo;

int esNumero(struct dirent *entry)
{
    int reti; // Retorno al compilar la expresion regular
    // Compilar la expresión regular
    reti = regcomp(&regex, "^[0-9]+$", REG_EXTENDED);
    // Verficar si existe reti --Retorno al compilar la expresión regular
    if (reti)
    {
        printf("Error al compilar la expresión regular.\n");

        exit(1);
    }
    // Compilar la expresión regular
    reti = regcomp(&regex, "^[0-9]+$", REG_EXTENDED);
    // Verficar si existe reti --Retorno al compilar la expresión regular
    if (reti)
    {
        printf("Error al compilar la expresión regular.\n");

        exit(1);
    }
    // verificamos si es un directorio
    reti = regexec(&regex, entry->d_name, 0, NULL, 0);
    return reti;
}

int openDirectory(const char *ruta, const char *id, char *infoMem)
{
    struct dirent *entry; // Estructura dirent leer las entradas del directorio
    char path[50];        // Almacenar ruta del archivo (?)
    // Construir ruta del archivo
    snprintf(path, sizeof(path), "%s/%s/status", ruta, id);

    char info[2048] = ""; // información del archivo
    int reti;             // Retorno al compilar la expresion regular

    DIR *dir = opendir(ruta); // Puntero hacia un directorio abierto
    int contador = 0;         // Contadora
    FILE *fptr;               // puntero a algun lado (?)
    char content[2048];       // Contenido de cada linea del archivo status
    bool found = false;       // bandera (?)

    // Abrir directorio
    if (dir == NULL) // No existe
    {
        perror("No se puede abrir el directorio");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) // iteraciones de las entradas del directorio
    {
        // Excluimos los directorios '.' y '..'
        if (entry->d_name[0] != '.')
        {
            if (entry->d_type == DT_DIR)
            {
                reti = esNumero(entry);
                if (!reti)
                { // Verificar si es un directorio
                    if (strcmp(entry->d_name, id) == 0)
                    {
                        fptr = fopen(path, "r");
                        if (fptr == NULL)
                        {
                            strcpy(info, "No se pudo abrir el archivo de estado de proceso");
                            found = false;
                            break;
                        }
                        // Formateo
                        sprintf(info, "Pid: %s\n", id);

                        while (fgets(content, 2048, fptr))
                        // Listar información del proceso
                        {
                            if (strncmp("Name:", content, 5) == 0)
                            {
                                strcat(info, "Nombre del proceso: ");
                                strcat(info, content + 6);
                            }
                            else if (strncmp("State:", content, 6) == 0)
                            {
                                strcat(info, "Estado del proceso: ");
                                strcat(info, content + 7);
                            }
                            else if (strncmp("VmSize:", content, 7) == 0)
                            {
                                strcat(info, "Tamaño de la imagen de memoria: ");
                                strcat(info, content + 8);
                            }
                            else if (strncmp("VmExe:", content, 6) == 0)
                            {
                                strcat(info, "Tamaño de la memoria TEXT: ");
                                strcat(info, content + 7);
                            }
                            else if (strncmp("VmData:", content, 7) == 0)
                            {
                                strcat(info, "Tamaño de la memoria DATA: ");
                                strcat(info, content + 8);
                            }
                            else if (strncmp("VmStk:", content, 6) == 0)
                            {
                                strcat(info, "Tamaño de la memoria STACK: ");
                                strcat(info, content + 7);
                            }
                            else if (strncmp("voluntary_ctxt_switches:", content, 24) == 0)
                            {
                                strcat(info, "# de cambios de contexto voluntarios: ");
                                strcat(info, content + 25);
                            }
                            else if (strncmp("nonvoluntary_ctxt_switches:", content, 27) == 0)
                            {
                                strcat(info, "# de cambios de contexto no voluntarios: ");
                                strcat(info, content + 28);
                            }
                        }
                        found = true;
                        // Cerramos el archivo
                        fclose(fptr);
                        break;
                    }
                }
                else if (reti == REG_NOMATCH)
                {
                    printf("La carpeta no pertenece a un proceso.\n");
                }
                else
                {
                    char error_buf[100];
                    regerror(reti, &regex, error_buf, sizeof(error_buf));
                    printf("Error al hacer la comparación: %s\n", error_buf);
                }
            }
        }
    }

    if (found == true)
    {
        if (infoMem != NULL)
        {
            strcpy(infoMem, info);
        }
        else
        {
            printf("- INFORMACIÓN DEL PROCESO - \n\n %s", info);
        }
    }
    else
    {
        if (infoMem != NULL)
        {
            sprintf(infoMem, "Pid: %s\n The ID was not found, the process does not exist. \n", id);
        }
        else
        {
            printf("The ID was not found, the process does not exist.");
        }
    }

    // Liberar la expresión regular compilada
    regfree(&regex);

    // Cerrar el directorio
    closedir(dir);
    // return contador;

    return found ? 1 : 0;
}

int esListaEsReporte(int argc, char *argv[])
{
    char lista[] = "-l";
    char reporte[] = "-r";
    FILE *archivoReporte;
    char path[] = "/proc/";
    char nombre[100] = "psinfo-report";
    char guion[100] = "-";
    ProcInfo *procInfos = NULL;
    int numProcesos = 0;

    if (argc > 2)
    {
        // Si el segundo argumento es -l se procede a leer los argumentos en lista o solo se busca un solo ID
        if (strcmp(argv[1], lista) == 0)
        {
            numProcesos = argc - 2;
            procInfos = (ProcInfo *)malloc(numProcesos * sizeof(ProcInfo));

            if (procInfos == NULL)
            {
                printf("Error: No se pudo asignar memoria.\n");
                return 1;
            }
            // Almacenar información de cada proceso en las estructuras
            for (int i = 0; i < numProcesos; i++)
            {
                strcpy(procInfos[i].pid, argv[i + 2]);
                openDirectory("/proc", argv[i + 2], procInfos[i].info);
            }

            // Mostrar info
            printf("-- Información recolectada!!! \n\n");
            for (int i = 0; i < numProcesos; i++)
            {
                printf("%s\n ", procInfos[i].info);
            }

            free(procInfos);
        }
        else if (strcmp(argv[1], reporte) == 0)
        {
            numProcesos = argc - 2;
            procInfos = (ProcInfo *)malloc(numProcesos * sizeof(ProcInfo));

            if (procInfos == NULL)
            {
                printf("Error: No se pudo asignar memoria.\n");
                return 1;
            }
            // Almacenar información de cada proceso en las estructuras
            for (int i = 0; i < numProcesos; i++)
            {
                strcpy(procInfos[i].pid, argv[i + 2]);
                strcat(nombre, guion);
                strcat(nombre, argv[i + 2]);
                openDirectory("/proc", argv[i + 2], procInfos[i].info);
            }
            strcat(nombre, ".info");
            archivoReporte = fopen(nombre, "w");

            // Mostrar info
            // printf("-- Información recolectada!!! \n\n");
            for (int i = 0; i < numProcesos; i++)
            {
                fputs(procInfos[i].info, archivoReporte);
                fputs("\n", archivoReporte);
            }
            fclose(archivoReporte);
            free(procInfos);
            printf("Archivo de salida generado: %s", nombre);
        }
        else
        { // No se encuentra el argumento -l
            printf("psinfo: usage error: Syntax error, -l is missing\n\n");
        }
    }
    else if (argc == 1)
    { // Si se ejecuta el comando ./psinfo solo sin argumentos
        printf("psinfo: usage error: Proccess ID required\n\n");
        printf("Options\n\n./psinfo ID --------------------------------> Returns the process info\n./psinfo -l ID1 ID2 ID3 ID4 ... IDn --------> Returns a report of processes info\n");
    }

    else
    { // Si tiene exactamente 2 argumentos se crea la ruta de la carpeta del proceso a buscar
        strcat(path, argv[1]);
        openDirectory("/proc", argv[1], NULL);
    }
    return 0;
}