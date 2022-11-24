#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "pss.h"
#include "disco.h"

// Defina aca sus variables globales
typedef struct {
int ready;
char *girlname;   //almacena nombre de mujer
char *boyname;    //almacena nombre de hombre
pthread_cond_t w; //almacena condicion
} Request;        //Struct de patron Request

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER; // Mutex

Queue *girlq; //Queue de mujeres
Queue *boyq;  // Queue de hombres


void DiscoInit(void) {
  // ... inicialice aca las variables globales ...
  girlq = makeQueue();
  boyq = makeQueue();
}

void DiscoDestroy(void) {
  // ... destruya las colas para liberar la memoria requerida ...
  destroyQueue(girlq);
  destroyQueue(boyq);
}

char *dama(char *nom) { //recibe nombre de dama
  // Si hay varones esperando pareja, dama retorna nombre del varon
  // que llego primero (primero en queue)
  //Si no hay varones, dama espera la invocacion de un varon
  pthread_mutex_lock(&m);

  char *localboy; //inicializamos pareja

  if(emptyQueue(girlq)){                                            //si queue propia esta vacia
      if(emptyQueue(boyq)){                                         //si la queue de la pareja esta vacia
          Request req = {0, nom, NULL, PTHREAD_COND_INITIALIZER};   //creamos struct
          put(girlq, &req);                                         //la ponemos en queue propio
          while(!req.ready){                                        //mientras no nos saquen a bailar
              pthread_cond_wait(&req.w, &m);                        //esperamos
          }
          localboy = req.boyname;   //si nos sacaron, nuestra pareja es la persona que nos saco del queue
      }

    else{
        Request *boy = get(boyq);           //Si la queue de pareja tiene gente, sacamos al que llego primero
        boy->ready = 1;                     //Le asignamos ready a la pareja
        boy->girlname = nom;                //obtenemos su nombre
        pthread_cond_signal(&boy->w);       //Damos señal a pareja
        localboy = boy->boyname;            //nuestra pareja es la persona que sacamos a bailar
    }
  }

  else{                                                       //Si hay gente en nuestro queue
    Request req = {0, nom, NULL, PTHREAD_COND_INITIALIZER};
    put(girlq, &req);                                         //creamos struct y nos metemos al queue
    while(!req.ready){                                        //mientras no nos saquen a bailar
        pthread_cond_wait(&req.w, &m);                        //esperamos
    }
    localboy = req.boyname;                                   //nuestra pareja es la persona que nos saca del queue
  }

  pthread_mutex_unlock(&m);
  return localboy;
}



char *varon(char *nom) { //recibe nombre de varon
  // Si hay damas esperando pareja, dama retorna nombre de la dama
  // que llego primero (primera en queue)
  //Si no hay damas, varon espera la invocacion de una dama
  pthread_mutex_lock(&m);
  char *localgirl;
   if(emptyQueue(boyq)){
       if(emptyQueue(girlq)){
           Request req = {0, NULL, nom, PTHREAD_COND_INITIALIZER};
           put(boyq, &req);
           while(!req.ready){
               pthread_cond_wait(&req.w, &m);
           }
           localgirl=req.girlname;
       }

       else{
           Request *girl = get(girlq);
           girl->ready = 1;
           girl->boyname = nom;
           pthread_cond_signal(&girl->w);
           localgirl = girl->girlname;
       }
   }
   else{
    Request req = {0, NULL, nom, PTHREAD_COND_INITIALIZER};
    put(boyq, &req);
    while(!req.ready){
        pthread_cond_wait(&req.w, &m);
    }
    localgirl = req.girlname;

  }
  pthread_mutex_unlock(&m);
  return localgirl;
}

