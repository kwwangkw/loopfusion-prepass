#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int main(){
    int a = rand() % 10 + 1;
    for(int i = 0; i < 10; i++){
        int j = i;
    }
    if (a == 2) {
        for(int i = 0; i < 10; i++){
            int k = i;
        }
    }
    return 0;
}

// '''
// int main(){
//     int a = rand() % 10 + 1;
//     for(int i = 0; i < 10; i++){
//         int j = i;
//         if (a == 2) {
//             int k = i;
//         }
//     }
//     return 0;
// }
// '''