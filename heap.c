#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "heap.h"
#include <string.h>

#define TAM_INICIAL 20
#define REDIMENSION 2
#define REMDIMENSION_ACHIQUE 4


struct heap{
	void** arreglo;
	size_t capacidad;
	size_t cantidad;
	cmp_func_t cmp_func;
};
//---------------FUNCIONES AUXILIARES-----------------

void swap(void** arreglo, size_t pos_a,size_t pos_b){
	void* auxiliar = arreglo[pos_a];
	arreglo[pos_a] = arreglo[pos_b];
	arreglo[pos_b] = auxiliar;
}

void up_heap(void** arreglo, size_t pos,size_t cant,cmp_func_t comparar){
	if(pos == 0){
		return;
	}
	size_t pos_padre =(pos-1)/2;
	if(comparar(arreglo[pos],arreglo[pos_padre])>0){ //si hijo mayor que el padre
		swap(arreglo,pos,pos_padre);
		up_heap(arreglo,pos_padre,cant,comparar);
	}

}

void down_heap(void** arreglo, size_t pos,size_t cant,cmp_func_t comparar){
	size_t pos_izq=  (2*pos)+1;
	size_t pos_der = (2*pos)+2;
	size_t maximo=pos;
	if(pos >= cant){ //si esta fuera de rango (corroborar rango)
		return;
	}
	if(pos_izq < cant && comparar(arreglo[pos],arreglo[pos_izq])<0){
		maximo = pos_izq;
	}
	if(pos_der < cant && comparar(arreglo[maximo],arreglo[pos_der])<0){
		maximo = pos_der;
	}
	if(pos != maximo){
		swap(arreglo,pos,maximo);
		down_heap(arreglo,maximo,cant,comparar);
	}

}

void heapify(void** arreglo,size_t cant,cmp_func_t comparar){
	size_t i = cant;
	for(; i>0; i--){
		down_heap(arreglo,i,cant,comparar);		
	}down_heap(arreglo,i,cant,comparar);
}

void heap_sort(void** arreglo,size_t cant,cmp_func_t comparar){
	heapify(arreglo,cant,comparar);
	size_t i = cant-1;
	for(; i > 0; i--){
		swap(arreglo,0,i);
		down_heap(arreglo,0,i,comparar);		
	}



}
void** copiar_arreglo(void** arreglo,size_t tam){
	void** nuevo_arreglo = malloc(sizeof(void*)*tam);
	if(!nuevo_arreglo){
		return NULL;
	}
	for(size_t i =0; i<tam;i++){
		nuevo_arreglo[i] = arreglo[i];
	}
	return nuevo_arreglo;
}

bool redimensionar(heap_t* heap, size_t nuevo_tam){
	void* arreglo_aux = realloc(heap->arreglo,sizeof(void*)*nuevo_tam);
	if(!arreglo_aux){
		return false;
	}
	heap->capacidad = nuevo_tam;
	heap->arreglo = arreglo_aux;
	return true;

}


//--------------PRIMITIVAS----------

heap_t *heap_crear(cmp_func_t cmp){
	heap_t* heap = malloc(sizeof(heap_t));
	if(!heap){
		return NULL;
	}
	void** heap_arr = malloc(sizeof(void*)* TAM_INICIAL);
	if(!heap_arr){
		free(heap);
		return NULL;
	}
	heap->arreglo = heap_arr;
	heap->capacidad = TAM_INICIAL;
	heap->cantidad = 0;
	heap->cmp_func = cmp;
	return heap;
}

heap_t *heap_crear_arr(void *arreglo[], size_t n, cmp_func_t comparar){
	heap_t* heap = malloc(sizeof(heap_t));
	if(!heap){
		return NULL;
	}
	void** nuevo_arreglo = copiar_arreglo(arreglo,n);
	if(!nuevo_arreglo){
		free(heap);
		return NULL;
	}
	heapify(nuevo_arreglo,n,comparar);
	heap->arreglo = nuevo_arreglo;
	heap->cantidad  =n;
	heap->cmp_func = comparar;
	heap->capacidad = n;
	return heap;

}

size_t heap_cantidad(const heap_t *heap){
	return heap->cantidad;
}

bool heap_esta_vacio(const heap_t *heap){
	return heap->cantidad ==0;
}

void heap_destruir(heap_t *heap, void destruir_elemento(void *e)){
	for(size_t i=0; i<heap->cantidad;i++){
		if(destruir_elemento){
			destruir_elemento(heap->arreglo[i]);
		}
	}
	free(heap->arreglo);
	free(heap);

}

bool heap_encolar(heap_t *heap, void *elem){
	if(heap->cantidad == heap->capacidad){
		if(!redimensionar(heap,heap->capacidad*REDIMENSION)){
			return false;
		}
	}
	heap->arreglo[heap->cantidad] = elem;
	heap->cantidad++;
	up_heap(heap->arreglo,heap->cantidad-1,heap->cantidad,heap->cmp_func);
	return true;
}

void *heap_ver_max(const heap_t *heap){
	if(heap_esta_vacio(heap)){
		return NULL;
	}
	return heap->arreglo[0];
}
void *heap_desencolar(heap_t *heap){
	
	if(heap_esta_vacio(heap))
		return NULL;
	void* dato = heap->arreglo[0];
	heap->arreglo[0] = heap->arreglo[heap->cantidad-1];
	heap->cantidad--;
	down_heap(heap->arreglo,0,heap->cantidad,heap->cmp_func);
	if((heap->cantidad < (heap->capacidad)/REMDIMENSION_ACHIQUE) && heap->capacidad > TAM_INICIAL){
		redimensionar(heap,heap->capacidad/REDIMENSION);
	}
	return dato;
}
