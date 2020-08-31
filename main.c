#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

void bufovfl(void);
void hello(void) ;
void countdown(unsigned n) ;


void
tell_main(void) {
    char buf[50];
    memset(buf, 0, sizeof(buf)) ;
    strcpy(buf, "hello, this is main");
    puts(buf) ;
}

int
main( int argc, char** argv) {
    char buf[70] ;
    memset(buf, 0, sizeof(buf)) ;
    strcpy(buf, "hello, this is main");
    puts(buf) ;
    tell_main(); 
    hello() ;
    countdown(5) ;
    bufovfl() ;
    return 0;
}

void
hello(void) {
    puts("hello") ;
}

void 
bufovfl(void) {
    int r = 0 ;
    char buf[20] ;
    setvbuf(stdin, 0, _IOFBF, 0);
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

char* s = "counting down : %u\n" ;
void
countdown(unsigned n) {
    char buf[50] ;
    uint64_t *p;
    memset(buf, 0, sizeof(buf));
    strcpy(buf, s) ;
    printf(buf, n) ;
    if ( n == 0 ) 
        return ;
    countdown(n-1) ;
    asm("leaq 0x8(%%rbp), %0;\n\t"
        : "=r"(p)
        :
        : );
    printf("retaddr saved in %p: %p\n", p, (void *) *p) ;
}



