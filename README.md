### Shadow Stack

## implementation detail

1. Memory Allocation
- **use system call in main**
- call libc function in main
- allocation in llvm built in: doInitialization()

2. Function Prologue/Epilogue
- **insert codes in the head of function and before ret instruction**
- PPCFrameLowering->emitPrologue/emitEpilogue
