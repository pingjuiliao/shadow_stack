#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>


void
mmap_libc(void) {
    void* addr = NULL ;
    addr = mmap(addr, 0x1000, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 
            -1, 0) ;
    memset(addr, 0, 0x1000) ;
    *((uint64_t*) addr) = 0x6867666564636261;
    printf("%s\n", (char *) addr) ;
    return ;
}

void
mmap_syscall(void) {
    void* addr = NULL ;
    asm volatile(
        "mov $0x9, %%rax;\n\t"
        "mov $0x0, %%rdi;\n\t"
        "mov $0x1000, %%rsi;\n\t"
        "mov $0x3, %%rdx;\n\t"
        "mov $0x22, %%r10;\n\t"
        "mov $0xffffffff, %%r8d;\n\t"
        "mov $0x0, %%r9d;\n\t"
        "syscall;\n\t"
        "mov %%rax, %0;\n\t"
        : "=m"(addr)
        : 
        : "rax", "rdi", "rsi", "rdx", "r10", "r8", "r9");
    memset(addr, 0, 0x100) ;
    *((uint64_t*) addr) = 0x6867666564636261;
    printf("%s\n", (char *) addr) ;
}


int
main(int argc, char** argv) {
    printf("sizeof(unsigned long) == 0x%lx\n", sizeof(unsigned long));
    printf("sizeof(size_t) == 0x%lx\n", sizeof(size_t));
    printf("sizeof(int)    == 0x%lx\n", sizeof(int));
    
    mmap_libc() ;
    mmap_syscall() ;
    return 0 ;
}
