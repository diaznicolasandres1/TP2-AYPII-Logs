#ifndef ANALOG_H
#define ANALOG_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool interfaz(char** comando, size_t M, abb_t* arbol_ips);
int comparar_ips(const char* ip1, const char* ip2);

#endif //ANALOG_H
