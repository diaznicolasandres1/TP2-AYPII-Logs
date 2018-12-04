#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

size_t contar_palabras(const char* str, char sep){
	size_t cont = 1;
	if(str == NULL)
		return 0;
	for(int i = 0;str[i]!='\0';i++){
		if(str[i]==sep)
			cont++;
	}
	return cont;
}
char* extraer_palabra(const char* str, size_t ini,size_t fin){
	size_t len_pal = fin - ini+1;
	char* palabra = malloc(sizeof(char)*(len_pal));
	if(palabra==NULL)
		return NULL;
		
	if(len_pal == 1){
		palabra[0] = '\0';
		return palabra;
	}
	for(size_t i = 0,j = ini; i<len_pal;i++,j++){
		palabra[i] = str[j];
	}
	palabra[len_pal-1] = '\0';
	return palabra;
}

char** split(const char* str, char sep){
	if(sep == '\0')
		return NULL;
	size_t ini = 0, fin = 0, cant_pal = 0,j = 0,len_str = 0; 
	len_str = strlen(str);
	cant_pal = contar_palabras(str,sep);
	char** lista_pal = malloc((sizeof(char*) * (cant_pal+1)));
	if(lista_pal == NULL)
		return NULL;
	
	for(size_t i = 0;i<len_str+1;i++){
		if(str[i] == sep || str[i] == '\0'){
			fin = i;
			char* palabra = extraer_palabra(str,ini,fin);
			if(palabra==NULL)
				return NULL;
			ini = fin+1;
			if(j<cant_pal){
				lista_pal[j] = palabra;
				j++;
			}
		}
	}
	lista_pal[cant_pal] = NULL;
	return lista_pal;	
}

char* join(char** strv, char sep){
	size_t cant_letras = 0;
	
	size_t i = 0;
	while(strv[i]!= NULL){
		cant_letras += strlen(strv[i]);
		i++;
	}
	if(i == 0){
		char* empty = malloc(sizeof(char));
		empty[0] = '\0';
		//printf("dev vacio\n");
		return empty;
	}
	char* arr = malloc(sizeof(char)*(cant_letras+i+1));
	if(!arr)
		return NULL;
	//arr[cant_letras+i+1] = '\0';
	
	size_t k = 0;
	for(size_t j = 0; j<i;j++){
		char* pal = strv[j];
		
		for(size_t l = 0;pal[l] != '\0';l++){
			arr[k] = pal[l];
			k++;
		}
		arr[k] = sep;
		k++;
	}

	arr[cant_letras+i-1] = '\0';
	return arr;
}

void free_strv(char* strv[]){
	for(int i = 0;strv[i]!=NULL;i++){
		char* palabra = strv[i];
		free(palabra);
	}
	free(strv);
}
