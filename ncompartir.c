#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER, WAIT_ACCEDER_TIMEOUT y
// WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.


// Defina aca sus variables globales.
// Para la cola de esperade nCompartir prefiera el tipo Queue.

    static NthQueue *accessq;
    static nThread *sharerth;
    static int busycounter;

// nth_compartirInit se invoca al partir nThreads para que Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {

    accessq = nth_makeQueue();
    sharerth = NULL;
    busycounter = 0;
}

void nCompartir(void *ptr) {

    START_CRITICAL

    nThread actualth = nSelf(); //permite obtener el thread
    //sharedptr = actualth->ptr;
    actualth -> ptr = ptr;
    sharerth = &actualth; //puntero global apunta al thread sharer
    while (!nth_emptyQueue(accessq)){ //despertamos a todos los threads en espera
        nThread th = nth_getFront(accessq);
        if(th->status == WAIT_ACCEDER_TIMEOUT){
            nth_cancelThread(th);
        }
        setReady(th);
    }

    suspend(WAIT_ACCEDER); //esperamos a que se invoque el ultimo devolver
    schedule();
    END_CRITICAL
}

static void f(nThread th) {
  // programe aca la funcion que usa nth_queryThread para consultar si
  // th esta en la cola de espera de nCompartir.  Si esta presente
  // eliminela con nth_delQueue.
  // Ver funciones en nKernel/nthread-impl.h y nKernel/pss.h
    if (nth_queryThread(accessq, th)){
        nth_delQueue(accessq, th);
        busycounter--;
    }

}

void *nAcceder(int max_millis) {
  // ...  use nth_programTimer(nanos, f);  f es la funcion de mas arriba
  /**
     Implementar el timeout para nAcceder.  
     Si millis es mayor o igual a 0 y después de millis milisegundos no hay ningún nCompartir,
     nAcceder termina la espera y retorna NULL.  Si millis es negativo, nAcceder espera por tiempo indefinido.
     * */
    START_CRITICAL
    nThread actualth = nSelf();
    void *localptr;
    busycounter ++;
    long long nanos = 1000000;
    nanos = nanos*max_millis;

    if(sharerth == NULL){ //si no se ha compartido un puntero
        nth_putBack(accessq, actualth); //nos ponemos en la cola de espera
        if (max_millis>0){
            suspend(WAIT_ACCEDER_TIMEOUT);
            nth_programTimer(nanos, f);
        }
        else{
            suspend(WAIT_COMPARTIR);
        }
        schedule();
    }
    nThread localsharerth;
    if (sharerth != NULL){
        localsharerth = *sharerth;
        localptr = localsharerth->ptr;
    }
    else{
        localptr = NULL;
    }
    END_CRITICAL
    return localptr;
}

void nDevolver(void) {

    START_CRITICAL
    busycounter--;
    if(busycounter == 0){
        setReady(*sharerth);
        sharerth = NULL;       //Resetea el thread global compartir
        schedule();
        
    }
    

    END_CRITICAL
}
