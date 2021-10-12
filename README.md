# Shadow Stack



shadow stack llvm pass (AMD64)
LLVM MachineFunctionPass cannot be built out-of-source 
I grab llvm-10.0.0 create another machine function pass.

## requirement
1. clang 10.0.0 up 
2. llvm  10.0.0 (included in this project)

## files
1. **./llvm/lib/Target/X86/X86ShadowStackPass.cpp** - instrumentation code 
2. **./main.c**                                     - simple c code for testing
3. ./llvm/lib/Target/X86/CMakeLists.txt         - make sure this file includes new pass (X86ShadowStackPass.cpp)
4. ./llvm/lib/Target/X86/X86.h                  - declare X86ShadowStack init functions
5. ./llvm/lib/Target/X86/X86TargetMachine.cpp   - add X86ShadowStackPass to it

## implementation detail
In the main function :
```
%rax   = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
arch_prctl(%rax, SET_GS) ;
%rbx   = mmap(NULL, STACKSIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0 );
mov %rax, 0x0(%rax) 
xor %rax, %rax 
xor %rbx, %rbx
```
shadow stack function prologue (except function named 'main')
```
mov 0x8(%rbp), %rbx
mov %gs:0x0, %rax
mov %rbx, 0x0(%rax)
add %rax, 8             // shadow stack grows upward
mov %rax, %gs:0x0
xor %rax, %rax 
```

shadow stack function epilogue (except function named 'main')
```
mov %gs:0x0, %rax 
sub %rax, 8
mov 0x0(%rax), %rbx
mov %rbx, 0x8(%rbp)
mov %rax, %gs:0x0
xor %rax, %rax 
```

## TODO
1. make shadow stack grows down ?
2. support 32-bit X86
3. more simplified code ?

## implementation choice
(Bold is the style I choose)
1. When to allocate memory ?
- FunctionPass - In runOnFunction(), if function name is main
- FunctionPass - doInitialization(Module &M)
- **MachineFunctionPass - runOnMachineFunction()**
- ModulePass   - runOnModule()

2. How to allocate memory ?
- IRBuilder - system call
- IRBuilder - libc function
- **Machine Instruction Builder - libc function**
- Machine Instruction Builder - system call
- some API in llvm ?

3. How to find function Prologue/Epilogue ?
- **insert codes in the head of function/before ret instruction**
- PPCFrameLowering->emitPrologue/emitEpilogue

4. What style of code should we insert ?
- traditional shadow stack - checking
- **traditional shadow stack - overwritting ( data except return address is still compromized )**
- parallel shadow stack

## other files
1. ./others/c-draft/ : some sample codes of how to use certain function in c and check Macros number
2. ./others/shadow-stack-llvm10 : compromised shadow stack using global variable, implemented in llvm-10
3. ./others/shadow-stack-llvm6  : compromised shadow stack using global variable, implemented in llvm-6

