#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) + 24 )
#define BUF_LEN     ( 1024 * EVENT_SIZE )

int main(int argc, char **argv) {
	char buffer[BUF_LEN];
	printf("Inicializando monitor de eventos.\n\n");
	// Al inicializar inotify este nos devuelve un descriptor de archivo, El file descriptor creado por inotify, es el que recibe la información sobre los eventos ocurridos
	int file_descriptor = inotify_init();
	if (file_descriptor < 0) {
		perror("inotify_init");
	}

	// Creamos un monitor sobre un path indicando que eventos queremos escuchar
	int watch_descriptor = inotify_add_watch(file_descriptor, "/home/danialjim/Escritorio/HOLA", IN_MODIFY | IN_CREATE | IN_DELETE);

	while (1) {
		read(file_descriptor, buffer, BUF_LEN);

		int offset = 0;
		// El buffer es de tipo array de char, o array de bytes. Esto es porque como los
		// nombres pueden tener nombres mas cortos que 24 caracteres el tamaño va a ser menor
		// a sizeof( struct inotify_event ) + 24.
		struct inotify_event *event = (struct inotify_event *) &buffer[offset];

		// El campo "len" nos indica la longitud del tamaño del nombre
		if (event->len) {
			// Dentro de "mask" tenemos el evento que ocurrio y sobre donde ocurrio
			// sea un archivo o un directorio
			if (event->mask & IN_CREATE) {
				if (event->mask & IN_ISDIR) {
					printf("Se creo la carpeta: %s .\n", event->name);
				} else {
					printf("Se creo el archivo: %s .\n", event->name);
				}
			} else if (event->mask & IN_DELETE) {
				if (event->mask & IN_ISDIR) {
					printf("Se elimino la carpeta: %s .\n", event->name);
				} else {
					printf("Se elimino el archivo: %s .\n", event->name);
				}
			} else if (event->mask & IN_MODIFY) {
				if (event->mask & IN_ISDIR) {
					printf("Se modifico la carpeta: %s .\n", event->name);
				} else {
					printf("Se modifico el archivo: %s .\n", event->name);
				}
			}
		}

	}

	inotify_rm_watch(file_descriptor, watch_descriptor);
	close(file_descriptor);

	return EXIT_SUCCESS;
}
