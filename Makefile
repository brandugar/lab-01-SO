# Reglas de compilación
psinfo: psinfo.o funciones.o
	gcc -o psinfo psinfo.o funciones.o

psinfo.o: psinfo.c
	gcc -c psinfo.c

fuciones.o: funciones.c funciones.h
	gcc -c funciones.c

clean: 
	rm -f *.o psinfo *.info