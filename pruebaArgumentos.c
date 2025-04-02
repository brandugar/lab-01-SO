#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <regex.h>

int openDirectory(const char *ruta, const char *id, const char *command) {
    struct dirent *entry;
    char path[50];
    snprintf(path, sizeof(path), "%s/%s/status", ruta, id);
    char info[2048];
    regex_t regex;
    int reti;
    DIR *dir = opendir(ruta);
    int contador = 0;
    FILE *fptr;
    char content[2048];
    
    
     // Compilar la expresión regular
    reti = regcomp(&regex, "^[0-9]+$", REG_EXTENDED);
    if (reti) {
        printf("Error al compilar la expresión regular.\n");
        exit(1);
    }
    // Abrir directorio
    if (dir == NULL) {
        perror("No se puede abrir el directorio");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Excluimos los directorios '.' y '..'
        if (entry->d_name[0] != '.') {
            if (entry->d_type == DT_DIR) {  // verificamos si es un directorio
                    reti = regexec(&regex, entry->d_name , 0, NULL, 0);
		    if (!reti) {
			//printf("La carpeta pertenece a un proceso.\n");
			//contador++;
			//printf("El nombre de la carpeta es -%s-\n",entry->d_name);
			// Buscamos si existe el proceso

			if (strcmp(entry->d_name, id) == 0) {
				//printf("SI EXISTE\n - INFORMACIÓN DEL PROCESO - \n\n");
				//system(command);
				
				fptr = fopen(path, "r");
				while(fgets(content, 2048, fptr)){
				 	if (strncmp("Name:", content, 5) == 0) {
					    strcat(info, "Nombre del proceso: ");
					    strcat(info, content + 6);
					} else if (strncmp("State:", content, 6) == 0) {
					    strcat(info, "Estado del proceso: ");
					    strcat(info, content + 7);
					} else if (strncmp("VmSize:", content, 7) == 0) {
					    strcat(info, "Tamaño de la imagen de memoria: ");
					    strcat(info, content + 8);
					} else if (strncmp("VmExe:", content, 6) == 0) {
					    strcat(info, "Tamaño de la memoria TEXT: ");
					    strcat(info, content + 7);
					} else if (strncmp("VmData:", content, 7) == 0) {
					    strcat(info, "Tamaño de la memoria DATA: ");
					    strcat(info, content + 8);
					} else if (strncmp("VmStk:", content, 6) == 0) {
					    strcat(info, "Tamaño de la memoria STACK: ");
					    strcat(info, content + 7);
					} else if (strncmp("voluntary_ctxt_switches:", content, 24) == 0) {
					    strcat(info, "# de cambios de contexto voluntarios: ");
					    strcat(info, content + 25);
					} else if (strncmp("nonvoluntary_ctxt_switches:", content, 27) == 0) {
					    strcat(info, "# de cambios de contexto no voluntarios: ");
					    strcat(info, content + 28);
					}
				}
		    	}
		    } else if (reti == REG_NOMATCH) {
			//printf("La carpeta no pertenece a un proceso.\n");
		    } else {
			char error_buf[100];
			regerror(reti, &regex, error_buf, sizeof(error_buf));
			printf("Error al hacer la comparación: %s\n", error_buf);
		    }

            }
        }
    }
    
    printf("INFO\n\n %s",info);
    
    // Cerramos el archivo
    fclose(fptr);
    
    //Liberar la expresión regular compilada
    regfree(&regex);
                

    closedir(dir);
    return contador;
}
int main(int argc, char* argv[]){
	char lista[] = "-l";
	int i;
	int j;
	bool itslist = false;
	char path[] = "/proc/";
	char command1[30] = "cat ";
	
	// Se verifica si el segundo argumento es -l en caso de tener más de 2 argumentos
	if(argc > 2 ){
		// Si el segundo argumento es -l se procede a leer los argumentos en lista o solo se busca un solo ID
		if(strcmp(argv[1], lista) == 0){
			itslist = true;
			for(i=1; i < argc; i++){
				printf("Argumento %d es %s \n", i, argv[i]);
				int a = atoi (argv[i]);
				printf("Valor de a %d \n",a-1);
			}
		}else{ // No se encuentra el argumento -l 
			printf("psinfo: usage error: Syntax error, -l is missing\n\n");
		}
		
	}else if(argc == 1){ // Si se ejecuta el comando ./psinfo solo sin argumentos
		printf("psinfo: usage error: Proccess ID required\n\n");
		printf("Options\n\n./psinfo ID --------------------------------> Returns the process info\n./psinfo -l ID1 ID2 ID3 ID4 ... IDn --------> Returns a report of processes info\n");
	}else{ // Si tiene exactamente 2 argumentos se crea la ruta de la carpeta del proceso a buscar
		strcat(path, argv[1]);
		strcat(command1,path);
		strcat(command1,"/status");
		openDirectory("/proc",argv[1], command1);
	}
	return 0;
}
