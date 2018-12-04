#define _POSIX_C_SOURCE  200809L
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "hash.h"
#include "lista.h"

#include "hashing.h"


#define FACTOR_REDIMENSION_SUP  5//Cuanto va a ser el tamanio de la redimension (ej: 2*tamanio del hash)
#define FACTOR_REDIMENSION_INF 2//Por cuanto vamos a dividir el tamaño del hash
#define HASH_TAM_TABLA 500
#define FACTOR_CARGA_INF 1
#define FACTOR_CARGA_SUP 5 
/*			Definicion de las estructuras			*/
struct hash{
	lista_t** listas;
	size_t cantidad_elem;
	size_t tamanio_tabla;	
	hash_destruir_dato_t destructor_dato; 	
};

typedef struct hash_campo{
	char* clave;
	void* dato;
}hash_campo_t;

struct hash_iter{
	const hash_t* hash; 
	lista_iter_t* lista_iter;    	
	size_t lista_actual;
	
};

/* FUNCIONES AUXILIARES */


hash_campo_t* crear_hash_campo(const char* clave, void* valor){
	hash_campo_t* campo = malloc(sizeof(hash_campo_t));
	if(!campo){
		return NULL;
	}	
	campo->clave = strdup(clave);
	if(!campo->clave){
		return NULL; 	
	}
	campo->dato = valor;	
	return campo;
}

lista_t** inicializar_listas(size_t tam){
	lista_t** listas = malloc(sizeof(lista_t*)*tam);
	if(!listas){
		return NULL;
	}
	for(size_t i=0;i<tam;i++){
		lista_t* lista = lista_crear();							
		if(!lista){  
			for(size_t j = 0; j<i-1 ; j++){
				lista_destruir(listas[j],NULL);
			} 
			free(listas);
			return NULL;
		}	
		listas[i] = lista;	
	} 
	return listas;
}

void hash_campo_destruir(hash_campo_t* campo,hash_destruir_dato_t destruir_dato){		
	if(destruir_dato){
		destruir_dato(campo->dato);
	}
	free(campo->clave);	
	free(campo);
}

//Redimensiona el hash al nuevo tamanio pasado por parametro
//PRE: El tamanio es siempre mayor a cero y el hash fue creado correctamente
//POS: devuelve true si se redimensiono correctamente, o false si fallo
bool hash_redimensionar(hash_t* hash, size_t nuevo_tam){
	if(nuevo_tam < HASH_TAM_TABLA){ // si nuevo tamaño es menor al tamaño inicial no redimensionamos
		return true;
	}	
	lista_t** nueva_tabla = inicializar_listas(nuevo_tam);
	if(!nueva_tabla){
		return false;
	}
	for(int i = 0;i<hash->tamanio_tabla;i++){
		lista_t* lista = hash->listas[i];
		while(!lista_esta_vacia(lista)){
			hash_campo_t* node = lista_borrar_primero(lista);
			char* clave = node->clave;
			long unsigned int new_pos = (long unsigned int) hashing(clave,nuevo_tam);
			lista_t* lista_a = nueva_tabla[new_pos];
			lista_insertar_ultimo(lista_a,node);
		}
		lista_destruir(hash->listas[i],NULL);
	}	
	lista_t** tabla_anterior = hash->listas;
	free(tabla_anterior);
	hash->listas = nueva_tabla;	
	hash->tamanio_tabla = nuevo_tam;		
	return true;
}


lista_iter_t* hash_obtener_iter(const hash_t *hash, const char *clave){			
	long unsigned int resultado = (long unsigned int) hashing(clave,hash->tamanio_tabla);
	lista_iter_t* iter = lista_iter_crear(hash->listas[resultado]);
	if(!iter){
		return NULL;
	}
	while(!lista_iter_al_final(iter)){
		hash_campo_t* nodo = lista_iter_ver_actual(iter);
		if(strcmp(nodo->clave,clave) == 0){
				
			return iter;
		}
		lista_iter_avanzar(iter);
	}
	return iter;	
}


/* ----------- IMPLEMENTACION DE PRIMITIVAS ----------- */


hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
	hash_t* nuevo_hash = malloc(sizeof(hash_t));
	if(!nuevo_hash ){
		return NULL;
	}	
	nuevo_hash->tamanio_tabla = HASH_TAM_TABLA;
	nuevo_hash->listas = inicializar_listas(HASH_TAM_TABLA);	
	if(!nuevo_hash->listas){
		free(nuevo_hash);
		return NULL;
	}	
	nuevo_hash->cantidad_elem = 0;
	nuevo_hash->destructor_dato = destruir_dato;
	return nuevo_hash;
}

size_t hash_cantidad(const hash_t *hash){	
	return hash->cantidad_elem;
}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){ 
	double fact_carga = (double) hash->cantidad_elem/  (double)hash->tamanio_tabla;
	if(fact_carga >= FACTOR_CARGA_SUP && FACTOR_CARGA_SUP > 0){ 
		hash_redimensionar(hash,(hash->tamanio_tabla)*FACTOR_REDIMENSION_SUP);		
	}
	lista_iter_t* iterador =  hash_obtener_iter(hash,clave);
	if(!iterador){
		return false;
	}
	/*Si ya existe esa clave en el hash*/
	if(!lista_iter_al_final(iterador)){ 
		hash_campo_t* campo_aux = lista_iter_ver_actual(iterador);
		void* dato_aux = campo_aux->dato;
		if(hash->destructor_dato){
			hash->destructor_dato(dato_aux);		
		}
		campo_aux->dato = dato;
		lista_iter_destruir(iterador);
		return true;				
	}
	/*Si es clave nueva*/
	hash_campo_t* campo=crear_hash_campo(clave,dato);
	if(!campo)
		return false;		
	lista_iter_insertar(iterador,campo);
	hash->cantidad_elem++;
	lista_iter_destruir(iterador);
	return true;
}
bool hash_pertenece(const hash_t *hash, const char *clave){
	lista_iter_t* iterador =  hash_obtener_iter(hash,clave);
	if(!iterador){
		return false;
	}	
	bool pertenece = !lista_iter_al_final(iterador);
	lista_iter_destruir(iterador);
	return pertenece;		
}


void* hash_borrar(hash_t *hash, const char *clave){	
	lista_iter_t* iterador  = hash_obtener_iter(hash,clave);		
	if(!iterador){			
		return NULL;
	}
	if(lista_iter_al_final(iterador)){
		lista_iter_destruir(iterador);
		return NULL;
	}
	hash_campo_t* borrado =lista_iter_borrar(iterador);
	void* dato = borrado->dato;
	hash_campo_destruir(borrado,NULL);
	hash->cantidad_elem--;
	lista_iter_destruir(iterador);
	/*Comprobamos si hay que redimensionar*/
	double fact_carga = (double) hash->cantidad_elem /  (double)hash->tamanio_tabla;
	if(fact_carga <= FACTOR_CARGA_INF && fact_carga>0){		 
		hash_redimensionar(hash,hash->tamanio_tabla/(size_t)FACTOR_REDIMENSION_INF);								
	}
	return dato;
}

void* hash_obtener(const hash_t *hash, const char *clave){
	lista_iter_t* iterador  = hash_obtener_iter(hash,clave);
	if(!iterador){
		return NULL;
	}	
	if(lista_iter_al_final(iterador)){
		lista_iter_destruir(iterador);
		return NULL;
	}
	hash_campo_t* campo = lista_iter_ver_actual(iterador);	
	void* dato = campo->dato;
	lista_iter_destruir(iterador);
	return dato;	
}

void hash_destruir(hash_t *hash){ 
	for(int i = 0;i<hash->tamanio_tabla;i++){
		lista_t* lista_actual = hash->listas[i];
		while(!lista_esta_vacia(lista_actual)){
			hash_campo_t* nodo = lista_borrar_primero(lista_actual);				
			hash_campo_destruir(nodo,hash->destructor_dato);	
		}
		lista_destruir(hash->listas[i],NULL);			
	}	
	free(hash->listas);
	free(hash);
}



/* -------------------------- HASH ITERADOR --------------------*/

hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* hash_iter = malloc(sizeof(hash_iter_t));
	if(!hash_iter){
		return NULL; 
	}		
	size_t i = 0;
	while((lista_esta_vacia(hash->listas[i]) ) &&  (i < hash->tamanio_tabla-1)){
		i++;
	}
	hash_iter->hash = hash;	
	hash_iter->lista_actual = i;	
	hash_iter->lista_iter = lista_iter_crear(hash->listas[i]); 
	if(!hash_iter->lista_iter){
		free(hash_iter);
		return NULL;
	}	
	return hash_iter;
}


bool hash_iter_avanzar(hash_iter_t *iter){	
	if(lista_iter_avanzar(iter->lista_iter) &&  !hash_iter_al_final(iter)){		
		return true;
	}
	iter->lista_actual++;	
	while(iter->lista_actual < iter->hash->tamanio_tabla){
		if(!lista_esta_vacia(iter->hash->listas[iter->lista_actual])){								
			lista_iter_destruir(iter->lista_iter); 
	             	iter->lista_iter = lista_iter_crear(iter->hash->listas[iter->lista_actual]);	             	
	             	return true;
	             }
	             iter->lista_actual++;	         
	}
	return false;	
}

/*Devuelve clave actual, esa clave no se puede modificar ni liberar*/
const char *hash_iter_ver_actual(const hash_iter_t *iter){	
	if(hash_iter_al_final(iter)){
		return NULL;
	}
	hash_campo_t* nodo = lista_iter_ver_actual(iter->lista_iter); 	
	return nodo->clave;
}



bool hash_iter_al_final(const hash_iter_t *iter){	
	if(!lista_iter_al_final(iter->lista_iter)){
		return false;
	}
	return true;	
}
 
void hash_iter_destruir(hash_iter_t* iter){
	lista_iter_destruir(iter->lista_iter);
	free(iter);
}




				
