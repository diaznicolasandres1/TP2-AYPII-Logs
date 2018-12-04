#include "pila.h"
#include <stdlib.h>
#include <stdio.h> //SACARLO
#define CAPACIDAD_INICIAL 10
#define MULTIPLO_AGRANDAR 2
#define MULTIPLO_ACHICAR 2
#define CRITERIO_ACHICAR 4
#define CRITERIO_AGRANDAR 2



/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void** datos;
    size_t cantidad;  // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

//FUNCIOINES AUXILIARES.

bool pila_redimensionar(pila_t* pila, size_t nuevo_tam){
	void* datos_aux = realloc(pila->datos, sizeof(void*)*(nuevo_tam));
	if(!datos_aux){
		return false;
	}
	pila->datos = datos_aux;
	pila->capacidad = nuevo_tam;
	return true; 
}




/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

pila_t* pila_crear(void){
	pila_t* pila = malloc(sizeof(pila_t));
	if(!pila){
		return NULL;
	}
	pila->datos = malloc(sizeof(void*)*CAPACIDAD_INICIAL);
	if(!pila->datos){
		free(pila);
		return NULL;
	}
	pila->cantidad = 0;
	pila->capacidad = CAPACIDAD_INICIAL;
	return pila;
}


void pila_destruir(pila_t *pila){	
	free(pila->datos);
	free(pila);	
}


bool pila_esta_vacia(const pila_t *pila){
	return (pila->cantidad == 0);
}

bool pila_apilar(pila_t *pila, void* valor){
	if(pila->cantidad == pila->capacidad){ 
		if(!pila_redimensionar(pila,pila->capacidad*MULTIPLO_AGRANDAR)){
			return false;
		}
	}
	pila->datos[pila->cantidad] = valor;	
	pila->cantidad++;
	return true;	
}
	

void* pila_ver_tope(const pila_t *pila){
	if(pila_esta_vacia(pila)){
		return NULL;
	}
	return pila->datos[pila->cantidad-1];
}	


void* pila_desapilar(pila_t *pila){
	if(pila_esta_vacia(pila)){
		return NULL;
	}
	void* dato  = pila_ver_tope(pila);
	pila->cantidad--;
	if((pila->cantidad * CRITERIO_ACHICAR < pila->capacidad) && pila->capacidad > CAPACIDAD_INICIAL){
		pila_redimensionar(pila,pila->capacidad/MULTIPLO_ACHICAR);
	}
	return dato;	
}

