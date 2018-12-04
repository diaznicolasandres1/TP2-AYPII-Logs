#ifndef LISTA_H
#define LISTA_H
#include <stdbool.h>
#include <stdlib.h>

typedef struct lista lista_t;
//Crea una lista enlazada vacia
//PRE: 
//POS: Devuelve un puntero a una lista enlazada o NULL si algo falla
lista_t *lista_crear(void);

//Devuelve si la lista esta vacia
//PRE: Recibe un puntero a lista enlazada
//POS: Devuelve True si la lista esta vacia, False en caso de que no este vacia
bool lista_esta_vacia(const lista_t *lista);

//Inserta un elemento al principio de la lista
//PRE: Recibe un puntero a una L.E y un puntero al dato que queremos agregar a la lista
//POS: Devuelve true si se inserto el elem. correctamente o false si no se inserto. 
bool lista_insertar_primero(lista_t *lista, void *dato);

//Inserta un elemento en la ultima pos. de una lista
//PRE: Recibe un puntero a lista y un puntero al dato a insertar
//POS: Devuelve true si se inserta correctamente, false si no. 
bool lista_insertar_ultimo(lista_t *lista, void *dato);

//Borra el primer elemento de la lista, devolviendo el dato
//PRE: Recibe un puntero a una lista
//POS: devuelve el puntero al dato de la primer posicion, o NULL si falla
void *lista_borrar_primero(lista_t *lista);

//Muestra el primer elemento de la lista
//PRE: recibe un puntero a lista enlazada
//POS: Devuelve el puntero al primer elemento de la lista o NULL si falla
void *lista_ver_primero(const lista_t *lista);

//Muestra el ultimo elemento de la lista
//PRE: Recibe un puntero a una lista enlazada
//POS: devuelve el puntero al ultimo elemento de la lista, o NULL si falla
void *lista_ver_ultimo(const lista_t* lista);

//Devuelve el largo de la lista
//PRE: Recibe un puntero a una lista enlazada
//POS: Devuelve la cantidad de elementos que contiene la lista.
size_t lista_largo(const lista_t *lista);

//Destruye una lista enlazada
//PRE: Recibe un puntero a una lista y un puntero funcion para destruir el dato que esta guardado 
//(NULL si no se requiere la funcion)
//POS: Destruye y libera la memoria pedida para la lista y sus elementos
void lista_destruir(lista_t *lista, void destruir_dato(void *));

//Aplica la funcion visitar a todos los elementos de la lista
//PRE: Recibe puntero a lista enlazada, una funcion de tipo bool y un parametro extra
//POS: Itera sobre todos los elementos de la lista aplicando la funcion visitar
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

//Definicion del iterador externo
typedef struct lista_iter lista_iter_t;

//Crear un iterador externo nuevo
//PRE: Recibe un puntero a una lista enlazada
//POS: Devuelve un puntero a un nuevo iterador
lista_iter_t *lista_iter_crear(lista_t *lista);

//Avanza el iterador a la proxima posicion
//PRE: recibe un puntero al iterador
//POS: Avanza la ref a actual al prox elemento de la lista
bool lista_iter_avanzar(lista_iter_t *iter);

//Muestra el valor de la posicion actual del iterador
//PRE: Recibe un puntero al iterador
//POS: devuelve un puntero al elemento actual o NULL si actual esta vacio.
void *lista_iter_ver_actual(const lista_iter_t *iter);

//Devuelve si el iterador esta al final de la lista
//PRE: Recibe un puntero al iter
//POS: Devuelve true si el iterador esta al final de la lista o false si no
bool lista_iter_al_final(const lista_iter_t *iter);

//Destruye el iterador
//PRE: Recibe un puntero a un iterador
//POS: Destruye y libera la memoria pedida para el iterador
void lista_iter_destruir(lista_iter_t *iter);

//Inserta un elemento en la posicion actual del iterador
//PRE: Recibe un puntero al iterador, y un puntero al elemento a guardar
//POS: Devuelve true si se inserto correctamente, o false si no
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

//Borra el elemento actual del iterador y devuelve su contenido
//PRE: Recibe un puntero al iterador
//POS: Devuelve el dato contenido en actual del iterador o NULL si esta vacio.
void *lista_iter_borrar(lista_iter_t *iter);

//PRUEBAS PARA LA LISTA ENLAZADA
void pruebas_lista_alumno(void);

#endif // LISTA_H
