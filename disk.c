#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "disk.h"
#include "priqueue.h"
#include "spinlocks.h"

// Le sera de ayuda la clase sobre semáforos:
// https://www.u-cursos.cl/ingenieria/2022/2/CC4302/1/novedades/detalle?id=431689
// Le serviran la solucion del productor/consumidor resuelto con el patron
// request y la solucion de los lectores/escritores, tambien resuelto con
// el patron request.  Puede substituir los semaforos de esas soluciones
// por spin-locks, porque esos semaforos almacenan a lo mas una sola ficha.

/*Un thread invoca requestDisk para solicitar acceso exclusivo al disco.
Debe esperar si el disco está ocupado. La identificación numérica de
pista, track, es linealmente proporcional a la distancia de la pista al
centro del disco. La pista 0 es la más cercana al centro. El thread
accederá a la pista track después del retorno de requestDisk. Luego
invocará releaseDisk notificando el término del uso del disco. Si en ese
momento hay varias solicitudes de acceso en espera, y se acaba de
acceder a la pista t, entre todos los requestDisk(t') pendientes Ud. debe
autorizar el acceso que requiera el cabezal en la pista t' más cercana a t
sujeto a que t' ≥ t. Autorice haciendo que ese requestDisk(t') retorne. Si
no hay ninguna solicitud con esas características, autorice la solicitud
que lleve el cabezal a la pista más cercana al centro del disco. Por
ejemplo si el cabezal está en la pista 4 y hay solicitudes en espera para
las pistas 2, 2, 3, 4, 6 y 10, el orden de autorización debe ser 4, 6, 10, 2,
2 y 3 (si no se hicieron otras solicitudes).

Recuerde que los spin-locks son semáforos binarios, es decir el
número de fichas puede ser solo 0 o 1. Para resolver el problema le será
de gran ayuda la clase sobre semáforos. En los ejemplos del patrón
request para semáforos el número de fichas no excede 1, y por lo tanto
los semáforos de esos ejemplos pueden ser substituidos por spin-locks.
Use una variable global que almacene la última pista t a la que se
autorizó el acceso. Almacene en una cola de prioridad todas las
solicitudes a pistas mayores o iguales a t, y otra cola de prioridad todas
las solicitudes a pistas menores que t. Las colas de prioridad vienen
implementadas en el archivo priqueue.c, con encabezados en priqueue.h.*/


// Declare los tipos que necesite
// ...
typedef struct {
int spin;
int track;
} Request;

// Declare aca las variables globales que necesite
// Prioqueue mayor o igual a t
// Prioqueue menor a t
// Ultima pista t a la que se autorizo acceso

//VARS GLOBALES
PriQueue *priomayor = NULL;
PriQueue *priomenor = NULL;
int t;
int mutex;


void iniDisk(void) {
  // Función de inicialización
  t=-1;
  mutex = OPEN;
  priomayor = makePriQueue();
  priomenor = makePriQueue();
}

void requestDisk(int track) {
  // Solicita acceso al disco indicando la pista

  spinLock(&mutex);             //lock
  if (t<0) {                    //si esta liberado esta liberado
    t=track;                    //track actual
    spinUnlock(&mutex);         //desbloqueamos mutex
    
  }

  else{
    Request req = {CLOSED, track};  //creamos request con spinlock y track
    if (track >= t){                //si track es mayor o igual a track global
    priPut(priomayor, &req, track); //lo ponemos en queue mayor
    }
    else{                           //si no, en queue menor
      priPut(priomenor, &req, track);
    }
    spinUnlock(&mutex);             //desbloqueamos
    spinLock(&req.spin);            //bloqueamos el
    
  }
}
  

void releaseDisk() {
  // Notificación de término de uso del disco

  spinLock(&mutex);                       //lock
  
  if (!emptyPriQueue(priomayor)){         //Esta la cola mayor vacia?
    Request *localreq = priGet(priomayor);//Sacar valor siguiente
    t = localreq->track;                  //poner su track global
    spinUnlock(&localreq->spin);          //desbloquearlo
  }

  else if (!emptyPriQueue(priomenor)){    //esta la cola menor vacia?
    Request *localreq = priGet(priomenor);//sacar primer valor
    t = localreq->track;                  //ponerlo en track global
    spinUnlock(&localreq->spin);          //desbloquearlo
    
    while (!emptyPriQueue(priomenor)){    //cambiar todo de la cola menor a cola mayor
      Request *actual = priGet(priomenor);  
      priPut(priomayor, actual, actual->track);
      
    }
  }

  else{                                   //si ambas colas estan vacias
    t=-1;                                 //reiniciamos el track
  }

spinUnlock(&mutex);                       //Unlock
}
