#define _POSIX_C_SOURCE 200809L  // Para strdup()
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "abb.h"
#include "pila.h"

//===========|Definicion de los structs|=============
typedef struct abb_nodo abb_nodo_t;
struct abb{

	abb_nodo_t* raiz;
	size_t cant_nodos;
	abb_comparar_clave_t cmp_clave;
	abb_destruir_dato_t destructor_dato;
};
struct abb_nodo{
	char* clave;
	void* dato;
	abb_nodo_t* izq;
	abb_nodo_t* der;
};
/*===================|Func. aux del ABB|=====================*/

/*ABB NODO CREAR*/
abb_nodo_t* abb_nodo_crear(const char* clave, void* dato){
	abb_nodo_t* nodo = malloc(sizeof(abb_nodo_t));
	if(!nodo)
		return NULL;
	char* clave_cpy = strdup(clave);
	if(!clave_cpy){
		free(nodo);
		return NULL;
	}
	nodo->clave = clave_cpy;
	nodo->dato = dato;
	nodo->izq = NULL;
	nodo->der = NULL;
	return nodo;
}


/*ABB NODO DESTRUIR*/
void* abb_nodo_destruir(abb_nodo_t* nodo){
	void* dato = nodo->dato;
	free(nodo->clave);		
	free(nodo);	
	return dato;
}



/*ABB ELIMINAR NODO*/
void eliminar_nodo(abb_nodo_t* nodo, abb_destruir_dato_t destruir){
	void* dato = abb_nodo_destruir(nodo);
	if(destruir)
		destruir(dato);	
}



/*ABB BUSCAR PADRE*/
abb_nodo_t* abb_buscar_padre(abb_nodo_t* nodo, const char* clave,abb_comparar_clave_t cmp){
	if(!nodo){		
		return NULL; 
	}	
	int comp = cmp(nodo->clave,clave);	
	if(comp < 0){  
        if(!nodo->der)
			return nodo;
		if(cmp(nodo->der->clave,clave) == 0)
			return nodo;
    	return abb_buscar_padre(nodo->der, clave, cmp);
	} 
	if(comp >0){
        if(!nodo->izq)
			return nodo;
        if(cmp(nodo->izq->clave,clave) == 0)
				return nodo;
    	return abb_buscar_padre(nodo->izq, clave, cmp);
    	}
	return NULL;
}



/*ABB NODO BUSCAR*/
abb_nodo_t* abb_nodo_buscar(abb_nodo_t *nodo,const char* clave, abb_comparar_clave_t cmp){
	if(!nodo)    	
    	return NULL;
    int comp = cmp(nodo->clave, clave);
    if(comp == 0) 
    	return nodo;
    if(comp < 0){     		       		
        return abb_nodo_buscar(nodo->der,clave,cmp);
    }
    if(comp > 0){    		
    	return abb_nodo_buscar(nodo->izq,clave,cmp);
    }
    return NULL;
}



/*ABB NODO INSERTAR*/
bool abb_nodo_insertar(abb_nodo_t* nodo,const char* clave, void* dato,abb_comparar_clave_t cmp,abb_destruir_dato_t destruir_dato,abb_t* arbol){
	int comp = cmp(nodo->clave,clave);
	if(comp == 0){
		void* old_dato = nodo->dato;
		if(destruir_dato)
			destruir_dato(old_dato);
		nodo->dato = dato;
		return true;
	}

	abb_nodo_t* padre = abb_buscar_padre(nodo,clave,cmp);
	if(!padre){
		return false;
	}	
	comp = cmp(padre->clave,clave);
	if(comp > 0){
		if(padre->izq){
			return abb_nodo_insertar(padre->izq,clave,dato,cmp,destruir_dato,arbol);
		}
		padre->izq = abb_nodo_crear(clave,dato);				
	}	
	else if(comp < 0){		
		if(padre->der){
			return abb_nodo_insertar(padre->der,clave,dato,cmp,destruir_dato,arbol);
		}
		padre->der = abb_nodo_crear(clave,dato);				
	}	
	arbol->cant_nodos++;
	return true;
}



/*ABB DESTRUIR NODOS*/
void abb_destruir_nodos(abb_nodo_t* nodo,abb_destruir_dato_t destruir_dato){
    if(!nodo)
    	return;
    abb_destruir_nodos(nodo->izq, destruir_dato);
    abb_destruir_nodos(nodo->der, destruir_dato);
    eliminar_nodo(nodo,destruir_dato);
}



/*ABB RECORRER IN ORDER*/
bool abb_recorrer_inorder(abb_nodo_t* nodo, bool visitar(const char *, void *, void *), void* extra){
	if(!nodo)
		return true;
	if(nodo->izq && !abb_recorrer_inorder(nodo->izq, visitar, extra))
		return false;
	if(!visitar(nodo->clave, nodo->dato, extra))
		return false;
	if(nodo->der && !abb_recorrer_inorder(nodo->der, visitar, extra))
		return false;
	return true;
}



/*ABB GET MININMO*/
abb_nodo_t* abb_get_min(abb_nodo_t* nodo){	
	if(!nodo->izq)
		return nodo;
	return abb_get_min(nodo->izq);
}




/*===================|Primitivas del ABB|====================*/



/*ABB CREAR*/
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){ 
	abb_t* abb = malloc(sizeof(abb_t));
	if(!abb)
		return NULL;
	abb->cant_nodos = 0;
	abb->cmp_clave = cmp;
	abb->destructor_dato = destruir_dato;
	abb->raiz = NULL;
	return abb;
}



/*ABB GUARDAR*/
bool abb_guardar(abb_t *arbol, const char *clave, void *dato){
	if(!arbol->raiz){
		abb_nodo_t* raiz = abb_nodo_crear(clave,dato);
		if(!raiz)
			return false;
		arbol->raiz = raiz;
		arbol->cant_nodos++;
		return true;
	}
	return abb_nodo_insertar(arbol->raiz,clave,dato,arbol->cmp_clave,arbol->destructor_dato,arbol);		
}




/*ABB BORRAR RECURSIVO*/
void* abb_borrar_r(abb_t* arbol,abb_nodo_t* nodo_raiz, const char *clave){	
	if(!nodo_raiz){
		return NULL;
	}
	abb_nodo_t* nodo = abb_nodo_buscar(nodo_raiz,clave,arbol->cmp_clave);
	if(!nodo){
		return NULL;
	}
	abb_nodo_t* padre = abb_buscar_padre(nodo_raiz,clave,arbol->cmp_clave);		
	if(!(nodo->izq && nodo->der) ){ 
		if(!padre){
			if(!!nodo->izq  && !nodo->der)
				arbol->raiz = NULL;
			if(!nodo->der)
				arbol->raiz = nodo_raiz->izq;
			else
				arbol->raiz = nodo_raiz->der;	
		}
		else if(padre->izq == nodo){
			if(!nodo->izq  && !nodo->der)
				padre->izq = NULL;
			if(!nodo->der)
				padre->izq = nodo->izq;
			else
				padre->izq = nodo->der;
		}	
		else if(padre->der == nodo){
			if(!nodo->izq  && !nodo->der)
				padre->der = NULL;
			if(!nodo->der)
				padre->der = nodo->izq;
			else			
				padre->der = nodo->der;
		}
	void* dato = abb_nodo_destruir(nodo);
	arbol->cant_nodos--;
	return dato;
	}
	//Si tiene dos hijos		
	abb_nodo_t* sucesor = abb_get_min(nodo->der);	
	char* clave_dup = strdup(sucesor->clave);
	if(!clave_dup)
		return NULL; 
	void* dato = nodo->dato; 	
	void* dato_suc = abb_borrar_r(arbol,arbol->raiz,clave_dup);
	if(arbol->destructor_dato)
		arbol->destructor_dato(nodo->dato);
	free(nodo->clave);
	nodo->clave = clave_dup;
	nodo->dato = dato_suc;		
	return dato;		
}




/*ABB BORRAR*/
void* abb_borrar(abb_t* arbol,const char *clave){
	if(!arbol)
		return NULL;
	if(!arbol->raiz)
		return NULL;	
	return abb_borrar_r(arbol,arbol->raiz,clave);
}



/*ABB OBTENER*/
void* abb_obtener(const abb_t *arbol, const char *clave){
	abb_nodo_t* node = abb_nodo_buscar(arbol->raiz,clave,arbol->cmp_clave);
	if(!node)
		return NULL;
	return node->dato;	
}



/*ABB PERTENECE*/
bool abb_pertenece(const abb_t *arbol, const char *clave){ 	
	return abb_nodo_buscar(arbol->raiz,clave,arbol->cmp_clave);
}



/*ABB CANTIDAD*/
size_t abb_cantidad(abb_t *arbol){ 	
	return arbol->cant_nodos;
}



/*ABB DESTRUIR*/
void abb_destruir(abb_t *arbol){ 
	abb_destruir_nodos(arbol->raiz,arbol->destructor_dato);
	free(arbol);
}



/*ABB RECORRER IN ORDER*/
void abb_in_order(abb_t* arbol, bool visitar(const char *, void *, void *), void *extra){
	//
	abb_recorrer_inorder(arbol->raiz, visitar, extra);
}



//===================|Iterador Externo|=======================


/*ESTRUCTURA DEL ITEDADOR*/ 
struct abb_iter{
	//
	pila_t* pila;
};



/*APILAR HIJOS IZQUIERDOS*/
void apilar_izquierdos(pila_t* pila, abb_nodo_t* nodo){
	if(nodo){
		pila_apilar(pila,nodo);
		apilar_izquierdos(pila,nodo->izq);
	}
}


/*ABB ITER CREAR*/
abb_iter_t* abb_iter_in_crear(const abb_t* arbol){
	if(!arbol)
		return NULL;
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	if(!iter)
		return NULL;
	
	pila_t* pila = pila_crear();
	if(!pila){
		free(iter);
		return NULL;
	}
	abb_nodo_t* raiz = arbol->raiz;
	if(!raiz){
		iter->pila = pila;
		return iter;
	}
	pila_apilar(pila,raiz);

	apilar_izquierdos(pila,raiz->izq);
	iter->pila = pila;
	return iter;
}



/*ABB ITER AVANZAR*/
bool abb_iter_in_avanzar(abb_iter_t *iter){
	if(abb_iter_in_al_final(iter))
		return false;
	abb_nodo_t* tope = pila_desapilar(iter->pila);
	if(tope->der != NULL){
		pila_apilar(iter->pila,tope->der);
		apilar_izquierdos(iter->pila,tope->der->izq);
	}
	return true;
}



/*ABB ITER VER ACTUAL*/
const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if(pila_esta_vacia(iter->pila))
		return NULL;
	abb_nodo_t* tmp = pila_ver_tope(iter->pila);
	return tmp->clave;
}



/*ABB ITER AL FINAL*/
bool abb_iter_in_al_final(const abb_iter_t *iter){
	return abb_iter_in_ver_actual(iter) == NULL;
}



/*ABB ITER DESTRUIR*/
void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila);
	free(iter);
}




/*===============================|FIN|==========================
Nicolas Andres Diaz   
*/