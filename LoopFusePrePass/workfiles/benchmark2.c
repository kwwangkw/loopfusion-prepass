#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


int main(){

    int a = 2;
    for(int i = 0; i < 10; i++){
        int j = i;
    }
    if (a == 2) {
        for(int i = 0; i < 10; i++){
            int k = i;
        }
    }

    int b = 2;
    for(int i = 0; i < 20; i++){
        int l = i;
    }
    if (b == 2) {
        for(int i = 0; i < 20; i++){
            int m = i;
        }
    }
    return 0;
}