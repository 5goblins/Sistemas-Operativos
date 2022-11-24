#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elim.h"

void eliminar_espacios(char *s){
    char *p1 = s;       //puntero 1
    char *p2 = s;       //puntero 2
    if (*p1 == '\0'){
        *p2 = '\0';
    }
    else{
        p1++;
        while(*p1 != '\0'){         //mientras no llegue al final del string
            if (*p1 != ' ') {       //si el puntero 1 no apunta a un espacio
                p2++;               // avanzamos el puntero 2
                *p2 = *p1;          // le damos el valor del puntero 1
                p1++;               // avanzamos el puntero 1
            }

            if (*p1 == ' '){        //si el puntero 1 apunta a un espacio
                if (*p2 != ' ') {   //si p2 no es un espacio
                    p2++;           //avanzamos p2
                    *p2 = *p1;      // ponemos un espacio
                }
                while (*p1 == ' '){ //si encontramos mas espacios seguidos
                    p1++;           // los saltamos. Asi llegamos al proximo caracter o al final del string
                }
            }
        }
        p2++;
        *p2= *p1;                   //ultimo reemplazo cuando p1 esta en 0
    }
}

char *eliminacion_espacios(const char *str) {
    ///////////////////////contador de espacios superfluos/////////////////////////////////////////////////////////////////////////////////////////
    /*const char *p1 = str;                            //puntero 1
    if (*p1 != '\0'){       
        p1++;                                        //comienza adelante del segundo
    }
    const char *p2 = str;                            //  puntero 2
    int counter = 0;                                 //  contador inicializado en 0
    while (*p1 != '\0') {                            //  mientras no llegue al final del string
        if (*p1 == ' ' && *p2 == ' ') {              //  si encontramos dos espacios seguidos
            counter++;                               //  agregamos 1 al contador de espacios superfluos
        }
        p1++;
        p2++;                                        //codigo viejo menos eficiente
    }*/
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //char *write = malloc(strlen(str) + 1 - counter); //  tamaño de string original + \0 - espacios superfluos//
    
    
    
    char copy[strlen(str)+1];                        //  puntero temporal para almacenar string
    strcpy(copy, str);                               //  copiamos el string
    eliminar_espacios(copy); 
    char*write = malloc(strlen(copy)+1);                        //  aplicamos eliminacion de espacios
    strcpy(write, copy);                             //  pasamos string sin espacios superfluos a la memoria reservada por malloc (en write)
    return write;                                    //  retornamos write
}



