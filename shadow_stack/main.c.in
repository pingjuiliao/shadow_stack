#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
uint32_t fibo(uint32_t n);

int
main( int argc, char** argv) {
    uint32_t input = 0;
    uint32_t result = 0;
    if ( argc < 2 ) {
        printf("Usage: %s <fibonacci_parameter>\n", argv[0]);
        exit(1);
    }
    input = atoi(argv[1]);
    result = fibo(input);
    printf("fibbonacci(%u) number is %u\n", input, result);
    return 0;
}

uint32_t 
fibo(uint32_t n) {
    if ( n <= 1 ) {
        return 1 ;
    }
    return fibo(n-1) + fibo(n-2) ;
}

