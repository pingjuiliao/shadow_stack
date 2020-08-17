#include "llvm/Pass.h"
#include "llvm/PassSupport.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/TargetPassConfig.h"
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
            errs() << MF.getName() << "\n" ;
            return false ;
        }
        
    };
} // namespace

char ShadowStackPass::ID = 0;
static RegisterPass<ShadowStackPass> X("shadowStackPass", "Shadow Stack Pass",false, false) ; 

// legacy PM registration

static RegisterStandardPasses Y(
        PassManagerBuilder::EP_EarlyAsPossible,
        [](const PassManagerBuilder &Builder,
            legacy::PassManagerBase &PM) {
                PM.add(new ShadowStackPass()) ; 
});
/*
namespace llvm {

    bool llvm::TargetMachine::addPassesToEmitFile(PassManagerBase &PM, raw_pwrite_stream &w, raw_pwrite_stream* pw, CodeGenFileType f ) {
        PM.add(new ShadowStackPass()) ;
        return true ;
    }

}*/
