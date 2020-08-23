#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <asm/prctl.h>
#include <errno.h>
#include <sys/prctl.h>
void
gsbase_libc(void) {
    unsigned long *in, *out ; 
    int r = 0 ;

    puts("In gsbase_libc") ;
    
    arch_prctl(ARCH_GET_GS, out);
    printf("\toriginal gsbase : %p\n", out);
    out = NULL ; 
    
    in = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0) ;
    if ( (r = arch_prctl(ARCH_SET_GS, (unsigned long) in)) != 0 ) {
        printf("\tsetting failed, errno : %s\n", strerror(errno));
        exit(-1) ;
    } 
    if ( (r = arch_prctl(ARCH_GET_GS, &out)) != 0 ) {
        printf("\tgetting failed, errno : %s\n", strerror(errno)) ;
        exit(-1) ;
    }
    printf("\tmmap   : %p\n", in); 
    printf("\tgsbase : %p\n", out);
 
    return; 
}

void
gsbase_syscall(void) {
    unsigned long *in = 0, *out= 0;
    puts("\nin gsbase_syscall") ;
    in = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
    asm volatile(
            "movq $158,    %%rax\n\t"
            //"movq $0x0,    %%rdi\n\t"
            "movq $4097, %%rdi\n\t"
            "movq %0  ,    %%rsi\n\t"
            "syscall\n\t" 
            "movq $158,    %%rax\n\t"
            //"movq $0x0,    %%rdi\n\t"
            "movq $4100, %%rdi\n\t"
            "leaq %1  ,    %%rsi\n\t"
            "syscall\n\t"
            :
            : "m"(in), "m"(out)
            :
            );
    printf("\tmmap  : %p\n", in);
    printf("\tgsbase: %p\n", out);
    return ;
}

int
main(void) {
    gsbase_libc() ;
    gsbase_syscall();
    return 0;
}



