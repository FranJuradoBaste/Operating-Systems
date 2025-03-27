#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char ** argv) {
    if (argc != 3) {
        printf("Usage: %s <ruta_fichero> <cadena_busqueda>\n", argv[0]);
        return -1;
    }

    return 0;
}
