#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "bolsa.h"
//GLOBAL VARS
pthread_mutex_t mut;        //Mutex
pthread_cond_t  cond;       //Condicion
int *globalstate = NULL;    //Estado del vendedor
int globalprice = 0;        //Precio
char *globalseller = NULL;  //nombre Vendedor
char *globalbuyer = NULL;   //nombre Comprador


int vendo(int precio, char *vendedor, char *comprador) {
    //ofrece accion a precio indicado
    //espera:
    //A que aparezca un comprador, retorna verdader o y copia el nombre del comprador en parametro 3
    //A que aparezca o halla ya un vendedor con precio menor, retornando falso.

    //Vendedor debe tener variable de local con su estado
    //0: Vigente, 1: Gano, -1: Perdio
    //Comprador asigna 1, vendedores con mejor precio asignan -1,

    int localstate;


    pthread_mutex_lock(&mut);

    if (precio< globalprice || globalprice == 0){ //Si tengo el mejor precio:
        localstate = 0;          // Soy vendedor actual
        if(globalstate != NULL){ // Si hay otro vendedor
            *globalstate = -1;   // Pierde
        }
        
        pthread_cond_broadcast(&cond); //Señalo que soy vendedor actual y despierto al antiguo
        globalprice = precio;          //Setters de variables globales
        globalseller = vendedor;
        globalstate = &localstate;
        globalbuyer = comprador;
    }
    else {                             //Si mi precio no es el mejor, retorno 0
        pthread_mutex_unlock(&mut);
        return 0;
    }

    while(localstate == 0){            //Mientras sea el vendedor activo
        pthread_cond_wait(&cond, &mut);//Espero
    }

    if(localstate == -1){              //Si mi estado cambia a -1
        pthread_mutex_unlock(&mut);
        return 0;                      //Me ganaron, retorno 0
    }
    else{                              //Else, si mi estado es 1
        pthread_mutex_unlock(&mut);
        return 1;                      //Me compraron, retorno 1
    }

}

int compro(char *comprador, char *vendedor) {
    // transa solo una accion con el vendedor mas barato
    // retorna precio pagado y copia el vendedor en parametro 2
    // si no hay vendedor precio es 0 y compro retorna

    pthread_mutex_lock(&mut);
    
    if (globalprice == 0){              //Si el precio actual es 0
        pthread_mutex_unlock(&mut);
        return 0;                       //Nadie esta vendiendo, retorno 0
    }
    
    strcpy(vendedor, globalseller);     //Setter de nombres comprador y vendedor
    strcpy(globalbuyer, comprador);

    *globalstate = 1;                   //cambio estado del vendedor a 1
    pthread_cond_broadcast(&cond);      //despierto al vendedor actual
    int localprice = globalprice;       //Variable local de precio
    //Reestablezco condiciones iniciables
    globalstate = NULL;
    globalprice = 0;
    globalseller = NULL;
    globalbuyer = NULL;
    pthread_mutex_unlock(&mut);

    return localprice;                  //retorno el precio guardado
}

