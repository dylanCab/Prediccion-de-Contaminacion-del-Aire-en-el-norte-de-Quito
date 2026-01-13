#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "validaciones.h"

int esEnteroPositivo(char *entrada) {
    for (int i = 0; entrada[i]; i++)
        if (!isdigit(entrada[i])) return 0;
    return atoi(entrada) > 0;
}

int esFlotantePositivo(char *entrada) {
    int punto = 0;
    for (int i = 0; entrada[i]; i++) {
        if (entrada[i] == '.') punto++;
        else if (!isdigit(entrada[i])) return 0;
    }
    return punto <= 1 && atof(entrada) > 0;
}

int esSoloLetras(char *entrada) {
    for (int i = 0; entrada[i]; i++)
        if (!isalpha(entrada[i]) && entrada[i] != ' ') return 0;
    return 1;
}

void limpiarBuffer() {
    while (getchar() != '\n');
}