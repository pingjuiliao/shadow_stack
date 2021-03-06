#include "X86.h"
#include "X86InstrInfo.h"
#include "X86InstrBuilder.h"
#include "X86Subtarget.h"
#include "X86RegisterInfo.h"
#include "X86TargetMachine.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Target/TargetMachine.h"
#include <vector>

#define X86_SHADOW_STACK_PASS_NAME "Dummy X86 Shadow Stack"

using namespace llvm;

// Project Name : X86ShadowStackPass
//   (replace this with your NewProjectName )

namespace {
    struct X86ShadowStackPass : public MachineFunctionPass {
        static char ID ;
        X86ShadowStackPass() : MachineFunctionPass(ID) {
            initializeX86ShadowStackPassPass(*PassRegistry::getPassRegistry()) ;
        }

        virtual bool runOnMachineFunction(MachineFunction &MF) {
            errs() << "this is " << MF.getName() << " function !\n" ; 
            
            // main func prologue
            if ( MF.getName() == "main" ) {
                createShadowStackSetup(MF) ;
                return true ;
            }

            // prologue
            createShadowStackPrologue(MF) ; 
            // epilogue
            for ( auto &MBB: MF ) {

                MachineInstr &MI = MBB.instr_back() ;
                if ( MI.isReturn() )  {
                    createShadowStackEpilogue(MF, MBB, MI) ;
                }
            }
            return true ;
        }
        void createShadowStackSetup(MachineFunction &MF) {
            const X86Subtarget &STI = MF.getSubtarget<X86Subtarget>() ;
            
            // Machine Function Info
            MachineBasicBlock &FirstMBB = MF.front() ;
            MachineBasicBlock::iterator MBBI = FirstMBB.begin() ;
            DebugLoc DL = FirstMBB.findDebugLoc(MBBI) ;
            const X86InstrInfo *X86II = STI.getInstrInfo() ;
            
            // void *gsbase = mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, 
            //                      MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) ;
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV64ri), X86::RDI)
                .addImm(0x0) ;
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV32ri), X86::ESI)
                .addImm(0x1000);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV32ri), X86::EDX)
                .addImm(0x3);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV32ri), X86::ECX)
                .addImm(0x22);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV32ri), X86::R8D)
                .addImm(0xffffffff);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV32ri), X86::R9D)
                .addImm(0x0);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::CALL64pcrel32))
                .addExternalSymbol("mmap");
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV64rr), X86::RBX)
                .addReg(X86::RAX, RegState::Define) ;

            // arch_prctl(ARCH_SET_GS, gsbase);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV64ri32), X86::RDI)
                .addImm(0x1001);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV64rr), X86::RSI)
                .addReg(X86::RBX);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::CALL64pcrel32))
                .addExternalSymbol("arch_prctl");

            // void *shadow_stack_top = mmap(NULL, 0x1000, ... ) 
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV64ri), X86::RDI)
                .addImm(0x0) ;
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV32ri), X86::ESI)
                .addImm(0x1000);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV32ri), X86::EDX)
                .addImm(0x3);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV32ri), X86::ECX)
                .addImm(0x22);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV32ri), X86::R8D)
                .addImm(0xffffffff);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV32ri), X86::R9D)
                .addImm(0x0);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::CALL64pcrel32))
                .addExternalSymbol("mmap");
            
            // *gsbase = shadow_stack_top ;
            // mov %shadow_stack_top, (%gsbase) 
            // ATT: mov %rax, 0x0(%rbx)
            addRegOffset(BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV64mr))
                     , X86::RBX, false, 0x0)
                .addReg(X86::RAX, RegState::Define);

            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::XOR64rr), X86::RBX)
                .addReg(X86::RBX, RegState::Undef)
                .addReg(X86::RBX, RegState::Undef);
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::XOR64rr), X86::RAX)
                .addReg(X86::RAX, RegState::Undef)
                .addReg(X86::RAX, RegState::Undef);
            
            // NOP for debug
            // BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::NOOP));
        }

        void createShadowStackPrologue(MachineFunction &MF) {
            
            const X86Subtarget &STI = MF.getSubtarget<X86Subtarget>() ;

            // Machine function Info
            MachineBasicBlock &FirstMBB = MF.front() ;
            MachineBasicBlock::iterator MBBI = FirstMBB.begin() ;
            DebugLoc DL = FirstMBB.findDebugLoc(MBBI) ;
            const X86InstrInfo *X86II = STI.getInstrInfo() ;
            
            // mov %gs:0x0, %rax
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV64rm), X86::RAX)
               .addReg(0x0)
               .addImm(0x1)
               .addReg(0x0)
               .addImm(0x0)
               .addReg(X86::GS);   // base, scale, Index, Disp, Segment 
            // 1) 
            // mov 0x8(%rbp), %rbx
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV64rm), X86::RBX)
                .addReg(X86::RBP)
                .addImm(0x1)
                .addReg(0x0)
                .addImm(0x8)
                .addReg(0x0);

            // mov %rbx, 0x0(%rax)
            addRegOffset(BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV64mr))
                    , X86::RAX, false, 0x0)
                .addReg(X86::RBX);
            
            // add %rax, 8
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::ADD64ri32), X86::RAX)
                .addReg(X86::RAX)
                .addImm(8) ;
            
            // mov %rax, %gs:0x0
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::MOV64mr))
                .addReg(0x0)
                .addImm(0x1)
                .addReg(0x0)
                .addImm(0x0)
                .addReg(X86::GS)
                .addReg(X86::RAX);
            // xor %rax, %rax
            BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::XOR64rr), X86::RAX)
                .addReg(X86::RAX, RegState::Undef)
                .addReg(X86::RAX, RegState::Undef);
            
            // NOP for debug 
            // BuildMI(FirstMBB, MBBI, DL, X86II->get(X86::NOOP)) ;
        }

        void createShadowStackEpilogue(MachineFunction &MF, MachineBasicBlock &thisMBB, MachineInstr &MI) {
            
            const X86Subtarget &STI = MF.getSubtarget<X86Subtarget>() ;
            const X86InstrInfo *X86II = STI.getInstrInfo() ;

            // Machine Function Info
            DebugLoc DL = MI.getDebugLoc() ;
            
            BuildMI(thisMBB, MI, DL, X86II->get(X86::NOOP)) ;
            BuildMI(thisMBB, MI, DL, X86II->get(X86::NOOP)) ;
            BuildMI(thisMBB, MI, DL, X86II->get(X86::NOOP)) ;
            BuildMI(thisMBB, MI, DL, X86II->get(X86::NOOP)) ;

            // mov %gs:0x0, %rax
            BuildMI(thisMBB, MI, DL, X86II->get(X86::MOV64rm), X86::RAX)
                .addReg(0)
                .addImm(1)
                .addReg(0)
                .addImm(0)
                .addReg(X86::GS) ;
            
            // sub %rax, 8
            BuildMI(thisMBB, MI, DL, X86II->get(X86::SUB64ri32), X86::RAX)
                .addReg(X86::RAX)
                .addImm(0x8);

            // mov 0x0(%rax), %rbx
            BuildMI(thisMBB, MI, DL, X86II->get(X86::MOV64rm), X86::RBX)
                .addReg(X86::RAX)
                .addImm(0x1)
                .addReg(0)
                .addImm(0x0)
                .addReg(0) ;

            // mov %rbx, 0x8(%rbp)
            BuildMI(thisMBB, MI, DL, X86II->get(X86::MOV64mr))
                .addReg(X86::RBP)
                .addImm(0x1)
                .addReg(0)
                .addImm(0x8)
                .addReg(0)
                .addReg(X86::RBX) ;
           
            // mov %rax, %gs:0x0
            BuildMI(thisMBB, MI, DL, X86II->get(X86::MOV64mr))
                .addReg(0)
                .addImm(0x1)
                .addReg(0)
                .addImm(0x0)
                .addReg(X86::GS)
                .addReg(X86::RAX) ;

            // xor %rax, %rax
            BuildMI(thisMBB, MI, DL, X86II->get(X86::XOR64rr), X86::RAX)
                .addReg(X86::RAX, RegState::Undef)
                .addReg(X86::RAX, RegState::Undef);
            

        }

        StringRef getPassName() const override {
            return X86_SHADOW_STACK_PASS_NAME ;
        }
        
    };
} // namespace

char X86ShadowStackPass::ID = 0;

INITIALIZE_PASS(X86ShadowStackPass, "my shadow stack pass",
    X86_SHADOW_STACK_PASS_NAME, true, true )

namespace llvm {
    FunctionPass *createX86ShadowStackPass() {
        return new X86ShadowStackPass() ;
    }
}
