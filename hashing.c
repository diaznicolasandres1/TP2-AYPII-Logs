#include <stdlib.h>
#include "hashing.h"
#define INITIAL_VALUE 0
#define MULT 33

size_t hashing(const char* clave, size_t capacidad){
   size_t clave_hash = INITIAL_VALUE;
   for(size_t i = 0; clave[i] !='\0'; i++)
      clave_hash = MULT * clave_hash + clave[i];
   return clave_hash % capacidad;

}


