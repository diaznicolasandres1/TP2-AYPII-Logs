#define _POSIX_C_SOURCE 200809L  // Para strdup() y getline
#define _XOPEN_SOURCE
#include <time.h>
#define TIME_FORMAT "%FT%T%z"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//HEADERS EXTERNOS
#include "strutil.h"
#include "heap.h"
#include "abb.h"
#include "hash.h"
#include "lista.h"
#include "analog.h"

int main(int argc,char* argv[]){
	if(argc < 2)
		return 0;
	size_t M = atoi(argv[1]);
	
	bool exit_flag = false;
	abb_t* arbol_ips = abb_crear(comparar_ips,free);
	if(!arbol_ips){
		return 0;
	}
	
	char* linea = NULL; size_t size = 0; ssize_t leidos;
	while((leidos  = getline(&linea,&size,stdin))>0){
		linea[leidos-1] = '\0';
		char** comando = split(linea,' ');
		
		exit_flag = interfaz(comando, M, arbol_ips);
		if(!exit_flag){
			fprintf(stderr,"Error en comando %s\n",comando[0]);
			free_strv(comando);
			break;
		}
		else
			fprintf(stdout,"OK\n");
		free_strv(comando);
	}
	abb_destruir(arbol_ips);
	free(linea);
	return 0;
}
