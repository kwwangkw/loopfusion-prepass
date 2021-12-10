#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main(){
    int a = 2;
    int foo [10000];
    for(int i = 0; i < 10000; i++){
        foo[i] = i;
    }
    if (a == 2) {
        for(int i = 0; i < 10000; i++){
            int k = foo[i];
        }
    }
    return 0;
}