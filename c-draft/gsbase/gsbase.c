#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <asm/prctl.h>
#include <errno.h>
#include <sys/prctl.h>
int
main(void) {
    unsigned long *in, *out ; 
    int r = 0 ;
    arch_prctl(ARCH_GET_GS, out);
    printf("gsbase : %p\n", out);
    out = NULL ; 
    
    puts("start setting !!!") ;
    in = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0) ;
    if ( (r = arch_prctl(ARCH_SET_GS, (unsigned long) in)) != 0 ) {
        printf("setting failed, errno : %s\n", strerror(errno));
        exit(-1) ;
    } 
    if ( (r = arch_prctl(ARCH_GET_GS, &out)) != 0 ) {
        printf("getting failed, errno : %s\n", strerror(errno)) ;
        exit(-1) ;
    }
    printf("gsbase : %p\n", out);
    printf("mmap   : %p\n", in); 
 
    return  0  ;
}
