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

//CONSTANTES
#define DOS_CANT 5 //la cantidad de accesos en x tiempo para determinar un DoS
#define DOS_TIME 2.0 //el tiempo entre accesos para determinar un DoS
#define TAM_MAX_LINEA 180


//==========|Structs|============

/*STRUCT RESGISTRO_T*/
typedef struct registro{
 char linea[TAM_MAX_LINEA];
 size_t n_particion;
}registro_t;

/*CREAR REGISTRO*/
registro_t* crear_registro(char* linea, size_t n_particion){
  /*uso calloc pq sino tengo problemas con fwrite*/
  registro_t* registro = calloc(1,sizeof(registro_t));
  if(!registro){
    return NULL;
  }
  strcpy(registro->linea,linea);
  registro->n_particion = n_particion;
  return registro;
}

//===========|Conversor de tiempo|===========

time_t iso8601_to_time(const char* iso8601){
	struct tm bktime = { 0 };
	strptime(iso8601, TIME_FORMAT, &bktime);
	return mktime(&bktime);
}

//==========|FUNCIONES DE COMPARACION|===========

/*Funcion comparar ips*/
int comparar_ips(const char* ip1, const char* ip2){		//TODO TRATAR DE MEJORAR ESTA NEGRADA
	int flag = 0;
	char** ip1_partes = split(ip1, '.');
	int ip1_campos[4];
	ip1_campos[0] = atoi(ip1_partes[0]);
	ip1_campos[1] = atoi(ip1_partes[1]);
	ip1_campos[2] = atoi(ip1_partes[2]);
	ip1_campos[3] = atoi(ip1_partes[3]);
	char** ip2_partes = split(ip2, '.');
	int ip2_campos[4];
	ip2_campos[0] = atoi(ip2_partes[0]);
	ip2_campos[1] = atoi(ip2_partes[1]);
	ip2_campos[2] = atoi(ip2_partes[2]);
	ip2_campos[3] = atoi(ip2_partes[3]);
	for(int i = 0; i < 4; i++){
		if(ip1_campos[i] > ip2_campos[i]){
			flag = 1;
			break;
		}
		if(ip1_campos[i] < ip2_campos[i]){
			flag = -1;
			break;
		}
	}
	free_strv(ip1_partes);
	free_strv(ip2_partes);
	return flag;
}

/*Funcion comparar dos lineas de registro*/
int comparar_lineas(char* linea1, char* linea2){
  char** campos1 = split(linea1,'	');
  char** campos2 = split(linea2,'	');
  size_t resultado = comparar_ips(campos1[0],campos2[0]);
  int retorno;
  time_t tiempo1 = iso8601_to_time(campos1[1]);
  time_t tiempo2 = iso8601_to_time(campos2[1]);
  if(difftime(tiempo1,tiempo2) > 0){
    retorno = 1;
  }
  else if(difftime(tiempo1,tiempo2) < 0){
    retorno = -1;
  }
  else if(resultado == 1){
    retorno = 1;
  }
  else if(resultado == -1){
    retorno = -1;
  }
  else retorno = strcmp(campos1[3],campos2[3]);
  free_strv(campos1);
  free_strv(campos2);
  return  -1*retorno;
}

/*Wrapper de funcion comparar_lineas*/
int func_comp(const void* a, const void* b){
 	return comparar_lineas((char*)a,(char*)b);
}

/*Funcion comparacion de registros*/
int func_comp_registro(const void* a, const void* b){
  registro_t* registro_1= (registro_t*)a;
  registro_t* registro_2= (registro_t*)b;
 	return comparar_lineas(registro_1->linea,registro_2->linea);
}

/*==========|ORDENAMIENTO EXTERNO|===========*/

/* @crear_particion
Crea una particion retorna la cantidad de lineas leidas
 retorna -1 si ocurre un error
 0 si no se lee ninguna linea.*/
size_t crear_particion(FILE* archivo_log, size_t tam_particion, size_t n_particion,char* nombre_salida){
  size_t total_lineas = 0;
  heap_t* heap = heap_crear(func_comp);
  if(!heap){
    return -1;
  }
  /*Encolo en un heap la cantidad de  lineas correspondientes a
  la particion*/
  char* linea_archivo=NULL;
  size_t capacidad =0,leidos=0;
  for(int i = 0 ; i < tam_particion ; i++){
    leidos = getline(&linea_archivo,&capacidad,archivo_log);
    if(leidos != -1){
      total_lineas++;
      linea_archivo[leidos-1] = '\0';
      char* linea =  strdup(linea_archivo);
      heap_encolar(heap,linea);
    }else{
      break;
    }
  }
  FILE* archivo_salida = fopen(nombre_salida,"wb");
  if(!archivo_salida){
    heap_destruir(heap,free);
    return -1;
  }
  /*Guardo en un archivo temporal la particion*/
  while(!heap_esta_vacio(heap)){
    char* linea = heap_desencolar(heap);
    registro_t* registro = crear_registro(linea,n_particion);
    if(!registro){
      heap_destruir(heap,free);
      fclose(archivo_salida);
      return -1;
    }
    fwrite(registro,sizeof(registro_t),1,archivo_salida);
    free(registro);
    free(linea);
  }
  fclose(archivo_salida);
  free(linea_archivo);
  heap_destruir(heap,NULL);
  return total_lineas;
}

/*@crear_pariciones
memoria disponibles son kb por eso multiplico por mil
retorna la cantidad de particiones que se hicieron*/
size_t crear_particiones(char* nombre_archivo, size_t memoria_disponible){
  size_t cant_lineas = (memoria_disponible*1000)/TAM_MAX_LINEA;
  FILE* archivo = fopen(nombre_archivo,"r");
  if(!archivo){
    return -1 ;
  }
  char nombre_archivo_salida[15];
  size_t cant_particiones = 1;
  size_t lineas_guardadas;
  while(!feof(archivo)){
    sprintf(nombre_archivo_salida,"archivo%zu.log",cant_particiones);
    lineas_guardadas = crear_particion(archivo,cant_lineas,cant_particiones,nombre_archivo_salida);
    if(lineas_guardadas > 0){
      cant_particiones++;
    }else{
      /*por como trabaja feof se ejecuta una vez de mas el while y
      se crea la misma particion final, por eso borro el ultimo archivo*/
      remove(nombre_archivo_salida);
      break;
    }
  }
  fclose(archivo);
  return cant_particiones-1;
}

/*@cerrar_archivos
cierra los archivos temporales*/
void  cerrar_archivos(FILE** archivos,size_t tam){
  for(size_t i = 0; i < tam ; i++){
    fclose(archivos[i]);
  }
  free(archivos);
}


/*@remove_archivos
remueve los archivos temporales*/
void remove_archivos(size_t cantidad_archivos){
  char nombre_archivo_particion[15];
  for(size_t i = 0; i < cantidad_archivos ; i++){
      sprintf(nombre_archivo_particion,"archivo%zu.log",i+1);
      remove(nombre_archivo_particion);
  }
}

/*@finalizar
cierra archivos, los rumueve, y destruye el heap*/
void finalizar(FILE** archivos,size_t cant_particiones,heap_t* heap){
  cerrar_archivos(archivos,cant_particiones);
  remove_archivos(cant_particiones);
  heap_destruir(heap,free);

}
/*@ordenar_archivo
ordena el archivo
*/
bool ordenar_archivo(char* nombre_archivo,char* nombre_salida, size_t memoria_disponible){
   char nombre_archivo_particion[15];
   size_t cant_particiones =  crear_particiones(nombre_archivo,memoria_disponible);
   heap_t* heap = heap_crear(func_comp_registro);
   if(!heap){
     return false;
   }
   FILE** archivos = calloc(cant_particiones,sizeof(FILE*));
   if(!archivos){
     heap_destruir(heap,free);
     return false;
   }
   for(int i = 0 ; i < cant_particiones ; i++){
      sprintf(nombre_archivo_particion,"archivo%i.log",i+1);
      FILE* archivo = fopen(nombre_archivo_particion,"rb");
      if(!archivo){
        heap_destruir(heap,free);
        cerrar_archivos(archivos,i-1);
        remove_archivos(cant_particiones);
        return false;
      }
      archivos[i] = archivo;
      registro_t* registro = malloc(sizeof(registro_t));
      fread(registro,sizeof(registro_t),1,archivo);
      heap_encolar(heap,registro);
   }
   FILE* salida = fopen(nombre_salida, "w");
   if(!salida){
     finalizar(archivos,cant_particiones,heap);
     return false;
   }
   while(!heap_esta_vacio(heap)){
     registro_t* registro = heap_desencolar(heap);
     size_t proviene = registro->n_particion;
     fprintf(salida,"%s\n",registro->linea);
     registro_t* leer = malloc(sizeof(registro_t));
     size_t resultado = fread(leer,sizeof(registro_t),1,archivos[proviene-1]);
     if(resultado > 0){
       heap_encolar(heap,leer);
     }else{
       free(leer);
     }
     free(registro);
   }
   fclose(salida);
   finalizar(archivos,cant_particiones,heap);
   return true;
}

//Wrapper para lista_destruir
void wrapper_lista_destruir(void* lista){
	lista_t* lista_ = lista;
	lista_destruir(lista_,NULL);
}
//Cuenta la cantidad de elementos dentro de un strv
size_t count_strv(char** strv){
	size_t cont = 0;
	if(!strv)
		return 0;
	while(strv[cont] != NULL)
		cont++;
	return cont;
}



//===========|FUNC PARA AGREGAR_ARCHIVO|==============

/*@hash_guardar_tiempos*/
bool hash_guardar_tiempos(hash_t* hash_tiempos, const char* ip, time_t tiempo){
	if(!hash_tiempos)
		return false;
	if(hash_pertenece(hash_tiempos, ip)){ //si la ip ya esta guardada
		lista_t* lista_tiempos_ip = hash_obtener(hash_tiempos, ip); //obtengo la lista de tiempos de esa ip
		lista_insertar_ultimo(lista_tiempos_ip, (void*)tiempo); //inserto el nuevo tiempo al final
		return true;
	}
	if(!hash_pertenece(hash_tiempos, ip)){ //Si la ip no pertenece al hash creo lista nueva y la guardo
		lista_t* new_lista = lista_crear();
		lista_insertar_ultimo(new_lista, (void*)tiempo);
		hash_guardar(hash_tiempos, ip, new_lista);
		return true;
	}
	return false;


}

/*@verificar_accesos*/
bool verificar_accesos(lista_t* lista_tiempo){
	lista_iter_t* iter1 = lista_iter_crear(lista_tiempo);
	lista_iter_t* iter2 = lista_iter_crear(lista_tiempo);
	int i = 0;
	while(i<DOS_CANT-1){ //Avanzo el iter2 N posiciones
		lista_iter_avanzar(iter2);

		if(lista_iter_al_final(iter2)){
			lista_iter_destruir(iter1);
			lista_iter_destruir(iter2);
			return false;
		}
		i++;
	}
	while(!lista_esta_vacia(lista_tiempo)){
		time_t tiempo1 = (time_t)lista_iter_ver_actual(iter1);
		time_t tiempo2 = (time_t)lista_iter_ver_actual(iter2);
		if(difftime(tiempo2,tiempo1) < DOS_TIME){
			lista_iter_destruir(iter1);
			lista_iter_destruir(iter2);
			return true;
		}
		lista_iter_avanzar(iter1);
		lista_iter_avanzar(iter2);
		if(lista_iter_al_final(iter2))
			break;
	}
	lista_iter_destruir(iter1);
	lista_iter_destruir(iter2);
	return false;
}

/*@check_dos */
abb_t* check_dos(hash_t* hash_tiempos){			//Devuelve un arbol con las ips que causaron DoS.
	abb_t* arbol_dos = abb_crear(comparar_ips,free);
	if(!arbol_dos){
		return NULL;
  }
	hash_iter_t* iter = hash_iter_crear(hash_tiempos);
	if(!iter){
		abb_destruir(arbol_dos);
		return NULL;
	}

	while(!hash_iter_al_final(iter)){
		const char* ip = hash_iter_ver_actual(iter);
		lista_t* lista = hash_obtener(hash_tiempos,ip);
		if(verificar_accesos(lista)){
			abb_guardar(arbol_dos,ip,NULL);
		}
		hash_iter_avanzar(iter);
	}
	hash_iter_destruir(iter);
	return arbol_dos;
}

/*@imprimir_ip_dos*/
bool imprimir_ip_dos(const char* clave, void* dato,void* extra){
	fprintf(stdout,"DoS: %s\n",clave);
	return true;
}

/*@imprimir_dos*/
void imprimir_dos(abb_t* arbol_dos){
	abb_in_order(arbol_dos, imprimir_ip_dos, NULL);
}

/*@agregar_archivo*/
bool agregar_archivo(char* nombre_archivo, abb_t* arbol_ips){
	if(!nombre_archivo){
		return false;
	}
	FILE* archivo = fopen(nombre_archivo,"r");
	if(!archivo){
		return false;
  }
	hash_t* hash_tiempos = hash_crear(wrapper_lista_destruir);
	if(!hash_tiempos){
		fclose(archivo);
		return false;
	}
	char* linea = NULL; size_t size = 0; ssize_t leidos;
	while((leidos  = getline(&linea,&size,archivo))>0){
		linea[leidos-1] = '\0';
		char** acceso = split(linea,'\t');
		char* ip = acceso[0];
		time_t hora = iso8601_to_time(acceso[1]);
		//char* metodo = acceso[2];
		//char* recurso = acceso[3];
		if(!abb_pertenece(arbol_ips,ip)){
			abb_guardar(arbol_ips,ip,NULL);
		}
		hash_guardar_tiempos(hash_tiempos,ip,hora);
		free_strv(acceso);
	}
	abb_t* arbol_dos = check_dos(hash_tiempos);
	if(!arbol_dos){
		hash_destruir(hash_tiempos);
		fclose(archivo);
		free(linea);
		return false;
	}
	if(abb_cantidad(arbol_dos) != 0){
		imprimir_dos(arbol_dos);
	}
	hash_destruir(hash_tiempos);
	abb_destruir(arbol_dos);
	free(linea);
	fclose(archivo);
	return true;
}

//==========|FUNC PARA VER VISITANTES|===========
bool imprimir_ip_rango(const char* clave, void* dato,void* extra){
	char** rango = extra;
	if(!(comparar_ips(clave,rango[0])<0) && !(comparar_ips(clave,rango[1])>0)) //si comparar ips devuelve 0 y esta dentro de rango imprime
		printf("\t%s\n",clave);
	return true;
}

bool ver_visitantes(abb_t* arbol_ips, char* desde, char* hasta){
	if(abb_cantidad(arbol_ips) == 0){
		return false;
	}
	char** rango = malloc(sizeof(char*)*2);
	rango[0] = desde;
	rango[1] = hasta;
	printf("Visitantes:\n");
	abb_in_order(arbol_ips, imprimir_ip_rango,(void*)rango);
	//fprintf(stdout,"OK\n" );
	free(rango);
	return true;
}


//==========|INTERFAZ|===========

bool interfaz(char** comando, size_t M, abb_t* arbol_ips){
	bool exit_flag = false;
	//printf("Arranco\n");
	//char** com_sep = split(comando, ' ');
	size_t cant_args = count_strv(comando);
	char* command = comando[0];
	if(strcmp(command, "ordenar_archivo") == 0){					//ORDENAR_ARCHIVO
		if(cant_args != 3){
			return exit_flag;
		}
		exit_flag = ordenar_archivo(comando[1],comando[2],M);
	}
	if(strcmp(command, "agregar_archivo") == 0){					//AGREGAR_ARCHIVO
		if(cant_args != 2){
			return exit_flag;
		}
		exit_flag = agregar_archivo(comando[1],arbol_ips);
	}
	if(strcmp(command, "ver_visitantes") == 0){					//VER_VISITANTES
		if(cant_args != 3){
			return exit_flag;
		}
		exit_flag = ver_visitantes(arbol_ips,comando[1],comando[2]);
	}

	return exit_flag;
}
