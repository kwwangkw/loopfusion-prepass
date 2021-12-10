#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main(){
    int a = 2;
    for(int i = 0; i < 10; i++){
        int j = i;
    }
    for(int i = 0; i < 10; i++){
        if (a == 2) {
            int k = i;
        }
    }
    return 0;
}
