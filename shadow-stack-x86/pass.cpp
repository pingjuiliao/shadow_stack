#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/Target/TargetMachine.h"
#include <vector>


using namespace llvm;

// Project Name : ShadowStack
//   (replace this with your NewProjectName )

namespace {
    struct ShadowStackPass : public MachineFunctionPass {
        static char ID ;
        bool debug ;
        ShadowStackPass() : MachineFunctionPass(ID) {}
         
        virtual bool runOnMachineFunction(MachineFunction &MF) {
            for ( MachineBasicBlock &MBB: MF ) {
                for ( MachineInstr &MI: MBB ) {
                    errs() << MI ;
                }
            }
            return false ;
        }
        
    };
}

char ShadowStackPass::ID = 0;
/*
static void
registerShadowStackPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new ShadowStackPass());
}
*/

static RegisterPass<ShadowStackPass> X("shadowStackPass", "Shadow Stack Pass",false, false) ; 
/*
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerShadowStackPass);
*/

static RegisterStandardPasses Y(
        PassManagerBuilder::EP_EarlyAsPossible,
        [](const PassManagerBuilder &Builder,
            legacy::PassManagerBase &PM) {
                PM.add(new ShadowStackPass()) ; 
});
