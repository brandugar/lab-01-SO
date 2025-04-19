#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <regex.h>
#include "funciones.h"

typedef struct
{
	char pid[16];
	char info[2048];
} ProcInfo;

// función para abrir el directorio

int main(int argc, char *argv[])
{
	char lista[] = "-l";
	char reporte[] = "-r";
	FILE *archivoReporte;
	bool itslist = false;
	char path[] = "/proc/";
	char nombre[100] = "psinfo-report";
	char guion[100] = "-";
	char command1[30] = "cat ";
	ProcInfo *procInfos = NULL;
	int numProcesos = 0;
	int j;

	// Se verifica si el segundo argumento es -l en caso de tener más de 2 argumentos
	if (argc > 2)
	{
		// Si el segundo argumento es -l se procede a leer los argumentos en lista o solo se busca un solo ID
		if (strcmp(argv[1], lista) == 0)
		{
			itslist = true;
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
				openDirectory("/proc", argv[i + 2], "", procInfos[i].info);
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
				openDirectory("/proc", argv[i + 2], "", procInfos[i].info);
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
		strcat(command1, path);
		strcat(command1, "/status");
		openDirectory("/proc", argv[1], command1, NULL);
	}
	return 0;
}
