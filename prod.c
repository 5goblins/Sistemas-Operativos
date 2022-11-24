#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "prod.h"

typedef struct {
    int *a;
    int i;
    int j;
    int p;
    BigNum *rizq;
} Args;

void *thread_function(void *p){
    Args *arg = (Args*) p; //casteo al tipo de mi struct
    arg -> rizq = parArrayProd(arg->a, arg->i, arg->j, arg->p);
    return NULL;
}

BigNum *parArrayProd(int a[], int i, int j, int p) {


    if (p == 1) { //caso base
       return seqArrayProd(a, i, j);
    }

    else {
        if (i == j) {
            return seqArrayProd(a, i, j); // Convierte un entero de C a BigNum
        } 
        int h = (i + j) / 2; // Pivote a la mitad
        pthread_t pid;
        Args args = {a, i, h, p / 2};
        pthread_create(&pid, NULL, thread_function, &args); //Thread izq
        BigNum *right = parArrayProd(a, h + 1, j, p - p / 2); //Orden paralelo der
        pthread_join(pid, NULL); //entierro thread
        BigNum *prod = bigMul(args.rizq, right); // Multiplicacion de BigNum's
        freeBigNum(args.rizq); // Hay que liberar la memoria ocupada por los BigNum's
        freeBigNum(right);
        return prod;
    }
}
