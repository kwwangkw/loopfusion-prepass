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
    else if (a == 3) {
        for(int i = 0; i < 10; i++){
            int k = i+1;
        }
    }
    else if (a == 4) {
        int k = 0;
    }
    else {
        for(int i = 0; i < 10; i++){
            int k = i+1;
        }
    }
    return 0;
}