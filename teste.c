#include <stdio.h>
#include <stdlib.h>

unsigned int vet[10] = {0};
int palavras = 0;

int main (){
    int i;

    for(i = 0; i < 10; i++){
        vet[i] += 1;
        printf("%d\n", vet[i]);
        palavras++;
    }

    printf("\n%d\n", palavras);
}
