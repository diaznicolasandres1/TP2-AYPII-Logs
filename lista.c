#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lista.h"

typedef struct nodo nodo_t;

struct nodo{
	void* dato;
	nodo_t* prox;
};

struct lista{
	nodo_t* prim;
	nodo_t* ult;
	size_t tamanio;
};
nodo_t* nodo_crear(void* dato){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if(nodo==NULL)
		return NULL;
	nodo->dato = dato;
	nodo->prox = NULL;
	return nodo;
}
/********************************************************************************************
 *							Primitivas de la Lista Enlazada									*
 ********************************************************************************************/

lista_t* lista_crear(void){
	lista_t* lista = malloc(sizeof(lista_t));
	if(lista == NULL)
		return NULL;
	lista->prim = NULL;
	lista->ult = NULL;
	lista->tamanio = 0;
	return lista;	
}
bool lista_esta_vacia(const lista_t* lista){
	return lista->tamanio == 0;
}
bool lista_insertar_primero(lista_t *lista, void *dato){
	nodo_t* nodo = nodo_crear(dato);
	if(nodo == NULL)
		return false;
	
	if(lista->tamanio == 0){
		lista->ult = nodo;
	}
	nodo->prox = lista->prim;
	lista->prim = nodo;
	lista->tamanio++;
	return true;				
}
bool lista_insertar_ultimo(lista_t *lista, void *dato){
	nodo_t* nodo = nodo_crear(dato);
	if(nodo == NULL)
		return false;
	if(lista->tamanio == 0){
		lista->prim = nodo;
	}
	else{
		nodo_t* ult = lista->ult;
		ult->prox = nodo;
	}
	lista->ult = nodo;
	lista->tamanio++;
	return true;
}
void* lista_borrar_primero(lista_t *lista){
	if (lista_esta_vacia(lista))
        return NULL;
    nodo_t* primero = lista->prim;
    void* valor = primero->dato;
    lista->prim = primero->prox;
    if(lista->ult == primero){
    	lista->ult = primero->prox;
    }
    free(primero);
    lista->tamanio--;
	return valor;
	
}
void *lista_ver_primero(const lista_t *lista){
	return lista_esta_vacia(lista)?NULL:lista->prim->dato;
}
void *lista_ver_ultimo(const lista_t* lista){
	return lista_esta_vacia(lista)?NULL:lista->ult->dato;
}
size_t lista_largo(const lista_t *lista){
	return (lista!=NULL)?lista->tamanio:0;
}
void lista_destruir(lista_t *lista, void destruir_dato(void *)){
	while(!lista_esta_vacia(lista)){
		if(destruir_dato)
			destruir_dato(lista_borrar_primero(lista));
		else 
			lista_borrar_primero(lista);
	}
	free(lista);
}

//Iterador interno

void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
	if(visitar==NULL)
		return;
	nodo_t* actual = lista->prim;
	while(actual!=NULL){
		if(!visitar(actual->dato,extra))
			return;
		actual=actual->prox;
	}		
}
/********************************************************************************************
 *									Iterador Externo 										*
 ********************************************************************************************/

struct lista_iter{
	nodo_t* actual;
	nodo_t* anterior;
	lista_t* r_lista;
};

lista_iter_t *lista_iter_crear(lista_t *lista){
	if(lista==NULL)
		return NULL;
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if(iter==NULL)
		return NULL;
	iter->r_lista = lista;
	iter->actual = lista->prim;
	iter->anterior = NULL;
	return iter;
	
}
bool lista_iter_avanzar(lista_iter_t *iter){
	if(lista_iter_al_final(iter))
		return false;
	iter->anterior = iter->actual;
	iter->actual=iter->actual->prox;
	return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter){
	return iter->actual == NULL?NULL:iter->actual->dato;
}
bool lista_iter_al_final(const lista_iter_t *iter){
	return iter->actual==NULL;//cuando esta al final es cuando actual es NULL no el ultimo.
}
void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}
bool lista_iter_insertar(lista_iter_t *iter, void *dato){		
	if(iter->anterior == NULL){//insertas primero siempre que actual sea el primero ==ARREGLADO==
		if(!lista_insertar_primero(iter->r_lista,dato))
			return false;	
		iter->actual = iter->r_lista->prim;
		return true;
		}
		
	if(lista_iter_al_final(iter)){
		if(!lista_insertar_ultimo(iter->r_lista,dato))
			return false;
		iter->actual = iter->r_lista->ult;
		return true;
	}
	nodo_t* nuevo_nodo = nodo_crear(dato);
	if(nuevo_nodo==NULL)
		return false;
	iter->r_lista->tamanio++;	
	iter->anterior->prox=nuevo_nodo;
	nuevo_nodo->prox = iter->actual;
	iter->actual=nuevo_nodo;//actual debe quedar apuntado al nodo que recien se inserto no anterior
	return true;
}
void *lista_iter_borrar(lista_iter_t *iter){
	if(lista_esta_vacia(iter->r_lista))
		return NULL;
	if(lista_iter_al_final(iter))
		return NULL;
	if(iter->anterior==NULL){
		void* dato = lista_borrar_primero(iter->r_lista);
		iter->actual = iter->r_lista->prim;
		return dato;
	}
	nodo_t* a_borrar = iter->actual;
	void* dato = a_borrar->dato;
	if(iter->actual->prox == NULL){
		iter->anterior->prox = NULL;
		iter->actual = NULL;
		iter->r_lista->ult = iter->anterior;
	}
	iter->anterior->prox = a_borrar->prox;
	iter->actual = a_borrar->prox;
	free(a_borrar);
	iter->r_lista->tamanio--;
	return dato;
}


