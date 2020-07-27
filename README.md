# Shadow Stack
shadow stack llvm pass

## implementation detail
(Bold is the style I choose)
1. When to allocate memory ?
- **FunctionPass - In runOnFunction(), if function name is main**
- FunctionPass - doInitialization(Module &M)
- MachineFunctionPass - runOnMachineFunction()
- ModulePass   - runOnModule()

2. How to allocate memory ?
- IRBuilder - system call
- **IRBuilder - libc function**
- some API in llvm ?

3. How to find function Prologue/Epilogue ?
- **insert codes in the head of function/before ret instruction**
- PPCFrameLowering->emitPrologue/emitEpilogue

4. What style of code should we insert ?
- **traditional shadow stack - checking**
- traditional shadow stack - overwritting
- parallel shadow stack

## other files
mmap/ : a sample code of how to use mmap in c
