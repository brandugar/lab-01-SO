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

int main(int argc, char *argv[])
{
	int rtrn = esListaEsReporte(argc, argv);
	return rtrn;
}
