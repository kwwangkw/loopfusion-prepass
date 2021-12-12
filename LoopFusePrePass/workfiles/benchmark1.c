#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main(){
    int a = 2;
    int j = 0;
    int k = 0;
    for(int i = 0; i < 100000; i++){
        j = i;
    }
    if (a == 2) {
        for(int i = 0; i < 100000; i++){
            k = i;
        }
    }
    printf("%d\n", k);
    return 0;
}