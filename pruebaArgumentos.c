#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <regex.h>

int openDirectory(const char *ruta, const char *id) {
    struct dirent *entry;
    regex_t regex;
    int reti;
    DIR *dir = opendir(ruta);
    int contador = 0;
    
    
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
			printf("La carpeta pertenece a un proceso.\n");
			contador++;
			printf("El nombre de la carpeta es -%s-\n",entry->d_name);
			// Buscamos si existe el proceso

			if (strcmp(entry->d_name, id) == 0) {
				printf("SI EXISTE.\n");
				
		    	}
		    } else if (reti == REG_NOMATCH) {
			printf("La carpeta no pertenece a un proceso.\n");
		    } else {
			char error_buf[100];
			regerror(reti, &regex, error_buf, sizeof(error_buf));
			printf("Error al hacer la comparación: %s\n", error_buf);
		    }

            }
        }
    }
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
	
	// Se verifica si el segundo argumento es -l en caso de tener más de 2 argumentos
	if(argc > 2 ){
		if(strcmp(argv[1], lista) == 0){
			itslist = true;
		}else{
			printf("psinfo: usage error: Syntax error, -l is missing\n\n");
		}
		
	}else{ // Si no tiene más de 2 argumentos se crea la ruta de la carpeta del proceso a buscar
		strcat(path, argv[1]);
	}
	
	
	// Si el segundo argumento es -l se procede a leer los argumentos en lista o solo se busca un solo ID
	if(itslist == true){
		for(i=1; i < argc; i++){
			printf("Argumento %d es %s \n", i, argv[i]);
			int a = atoi (argv[i]);
			printf("Valor de a %d \n",a-1);
		}
	}else if(itslist == false){
		openDirectory("/proc",argv[1]);
	}else{
		printf("psinfo: usage error: Proccess ID required\n\n");
		printf("Options\n\n./psinfo ID --------------------------------> Returns the process info\n./psinfo -l ID1 ID2 ID3 ID4 ... IDn --------> Returns a report of processes info\n");
	}
	return 0;
}
