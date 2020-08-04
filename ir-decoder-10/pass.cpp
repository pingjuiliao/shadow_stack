#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/GlobalValue.h"
#include <vector>


using namespace llvm;

// Project Name : IRDecode
//   (replace this with your NewProjectName )

namespace {
    struct IRDecodePass : public FunctionPass {
        static char ID ;
        IRDecodePass() : FunctionPass(ID) {}
        
        virtual bool runOnFunction(Function &F) {
            errs() << "\n\n#######################################\n" ;
            errs() << "#####    " << F.getName() ;
            errs() << "\n#######################################\n";
            for ( auto it = F.begin(), E = F.end() ; it != E ; ++it ) {
                errs() << *it << "\n" ;
            }
            return false ;
        }

    };
}

char IRDecodePass::ID = 0;
/*
static void
registerIRDecodePass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new IRDecodePass());
}
*/

static RegisterPass<IRDecodePass> X("shadowStackPass", "Shadow Stack Pass",
                                        false, false) ; 

/*
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerIRDecodePass);
*/

static RegisterStandardPasses Y(
        PassManagerBuilder::EP_EarlyAsPossible,
        [](const PassManagerBuilder &Builder,
            legacy::PassManagerBase &PM) {
                PM.add(new IRDecodePass()) ; 
});

