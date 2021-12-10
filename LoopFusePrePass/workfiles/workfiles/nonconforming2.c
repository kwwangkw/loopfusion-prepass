#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main(){
    int a = rand() % 10 + 1;
    for(int i = 0; i < 10; i++){
        int j = i;
    }
    for(int i = 0; i < 7; i=i+2){
        int k = i;
    }
    return 0;
}

'''
int main(){
    int a = rand() % 10 + 1;
    for(int i = 0; i < 10; i++){
        int j = i;
    }
    for(int i = 0; i < 10; i++){
        if (i % 2 == 0 && i < 8){
            int k = i;
        }
    }
    return 0;
}
'''