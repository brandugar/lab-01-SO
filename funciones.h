#ifndef FUNCIONES_H
#define FUNCIONES_H

// Función que abre el directorio de los procesos donde buscaremos el/los procesos que listaremos
int openDirectory(const char *ruta, const char *id, const char *command, char *infoMem);
// Función para comparar un string es un número
int esNumero(struct dirent *entry);
// Función que lista o realiza un reporte según sea la entrada del comando
int esListaEsReporte(int argc, char *argv[]);

#endif // FUNCIONES_H