#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
void bufovfl(void);

int
main( int argc, char** argv) {
    bufovfl() ;
    return 0;
}

void 
bufovfl(void) {
    int r = 0 ;
    char buf[20] ;
    memset(buf, 0, sizeof(buf)) ;
    printf("Please input your name :") ;    
    r = read(0, buf, 50) ; 
    if ( r == 0 ) {
        fprintf(stderr, "read() error\n") ;
        exit(-1) ;
    }
    printf("Welcome, %s\n", buf) ;
    return ;
}

