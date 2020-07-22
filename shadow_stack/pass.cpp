#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

// Project Name : ShadowStack
//   (replace this with your NewProjectName )

namespace {
    struct ShadowStackPass : public FunctionPass {
        static char ID ;
        ShadowStackPass() : FunctionPass(ID) {}
        
        // Allocate shadow stack memory 
        void createAllocationPrologue(Function &F) {
            BasicBlock &B   = F.getEntryBlock() ;
            Instruction *pi = B.getFirstNonPHI() ; 
            IRBuilder<> IRB(pi) ;
            
        }
        
        // function prologue
        void createShadowStackPrologue(void) {

        }
        
        // function epilogue
        void createShadowStackEpilogue(void) {

        }
        virtual bool runOnFunction(Function &F) {
            errs() << "I saw a function called " << F.getName() << "!\n" ;
            if ( F.getName() == "main" ) {
                createAllocationPrologue(F) ;
            } else {
                createShadowStackPrologue() ;
                createShadowStackEpilogue() ;
            }

            return false ;
        }
    };
}

char ShadowStackPass::ID = 0;

static void
registerShadowStackPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new ShadowStackPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerShadowStackPass);
