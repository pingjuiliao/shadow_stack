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

// Project Name : ShadowStack
//   (replace this with your NewProjectName )

namespace {
    struct ShadowStackPass : public FunctionPass {
        static char ID ;
        bool debug ;
        GlobalVariable* shadow_bound ;
        GlobalVariable* shadow_sp ;
        ShadowStackPass() : FunctionPass(ID) {}
        
        virtual bool doInitialization(Module &M) {
            LLVMContext& C = M.getContext() ;
            PointerType* ty= PointerType::get(Type::getInt8PtrTy(C), 0); 
            Constant* null = ConstantPointerNull::get(ty) ;
            
            debug = false ;
            /**************************
             *** Function Insertion
             *************************/
            M.getOrInsertFunction("mmap", FunctionType::get(Type::getInt8PtrTy(C), true) ) ;
            M.getOrInsertFunction("printf", FunctionType::get(Type::getInt32Ty(C), true) ) ;
            M.getOrInsertFunction("exit", FunctionType::get(Type::getInt32Ty(C), { Type::getInt32Ty(C) }, false)) ; 
            
            Intrinsic::ID retaddr_id  = Function::lookupIntrinsicID("llvm.returnaddress") ;
            Intrinsic::getDeclaration(&M, retaddr_id);

            /**************************
             *** Global Variable
             **************************/
            // i8** __shadow_stack_pointer ;
            shadow_sp = new GlobalVariable(M, ty, false, GlobalValue::CommonLinkage, null, "__shadow_stack_pointer");
            // i8** __shadow_stack_bound  ; 
            shadow_bound = new GlobalVariable(M, ty, false, GlobalValue::CommonLinkage, null, "__shadow_stack_bound") ;
            return false ;    
        }
        
        virtual bool runOnFunction(Function &F) {
            errs() << "I saw a function called " << F.getName() << "!\n" ;
            
            std::vector<Instruction*> ret_list ;
            if ( F.getName() == "main" ) {
                createAllocationPrologue(F) ;
            } else {
                createShadowStackPrologue(F) ;
                // search for epilogue (return instruction)
                for ( auto pbb = F.begin() ; pbb != F.end() ; pbb ++  ) {
                    for ( auto pi = pbb->begin() ; pi != pbb->end() ; pi ++ ) {
                        if ( auto ret_ins = dyn_cast<ReturnInst>(&*pi) ) {
                            // createShadowStackEpilogue(ret_ins) ;
                            ret_list.emplace_back(ret_ins) ;
                        }
                    }
                }
                for ( auto it = ret_list.begin() ; it != ret_list.end() ; ++it ) {
                    createShadowStackEpilogue(*it) ; 
                }
            }

            return false ;
        }

        // Allocate shadow stack memory 
        void createAllocationPrologue(Function &F) {
            BasicBlock &B   = F.getEntryBlock() ;
            Instruction *pi = B.getFirstNonPHI() ; 
            
            LLVMContext& C = pi->getContext() ;
            Module *M = pi->getModule() ;
            Constant* null      = ConstantPointerNull::get(PointerType::getInt8PtrTy(C)) ;
            
            IRBuilder<> IRB(pi) ; 
            FunctionCallee func_mmap   = M->getFunction("mmap") ;
            FunctionCallee func_printf = M->getFunction("printf") ;
            
            Value* hello = IRB.CreateGlobalStringPtr("Hello %d!!!\n") ; 
            Constant* zero = IRB.getInt32(0x0);
            std::vector<Value *> args_puts({hello, zero}) ;
            IRB.CreateCall(func_printf, args_puts);
            
            // p = mmap(...)
            Constant* mapped_size = IRB.getInt64(0x1000) ; 
            Constant* prot_rw     = IRB.getInt32(0x3) ;
            Constant* map_private = IRB.getInt32(0x22) ;
            Constant* fd          = IRB.getInt32(0xffffffff) ;
            std::vector<Value *> args_mmap({null, mapped_size, prot_rw, 
                                            map_private, fd, zero}) ;
            CallInst* call_mmap   = IRB.CreateCall(func_mmap, args_mmap);
            
            // bitcast i8* to i8**
            Type*  ty           = shadow_bound->getType()->getElementType() ;
            Value* bitcast_mmap = IRB.CreateBitCast(call_mmap, ty) ;
            
            // STORE shadow stack bound
            // TODO : hide this (instead of using global variable)
            IRB.CreateStore(bitcast_mmap, shadow_bound);
            
            // STORE shadow stack base
            Constant* base_shift   = IRB.getInt64((0x1000 - 8) / 8);
            Value* shadow_base     = IRB.CreateInBoundsGEP(bitcast_mmap, base_shift);
            IRB.CreateStore(shadow_base, shadow_sp);


            // printf DEBUG
            if ( debug ) {
                std::vector<Value*> arg_printf ;
                Value* bb_fstr =  IRB.CreateGlobalStringPtr("Bound: %p, Base: %p\n") ; 
                arg_printf.emplace_back(bb_fstr) ;
                LoadInst* ss_bound = IRB.CreateLoad(shadow_bound);
                arg_printf.emplace_back(ss_bound) ;
                LoadInst* ss_base  = IRB.CreateLoad(shadow_sp) ;
                arg_printf.emplace_back(ss_base) ;
                IRB.CreateCall(func_printf, arg_printf) ; 
                  
            }
        }
         
        // function prologue
        void createShadowStackPrologue(Function &F) {
            BasicBlock &B   = F.getEntryBlock() ;
            Instruction *pi = B.getFirstNonPHI() ; 
            Module *M = pi->getModule() ;
            
            // function callees
            FunctionCallee func_printf  = M->getFunction("printf"); 
            FunctionCallee func_retaddr = M->getFunction("llvm.returnaddress") ;
        
            
            IRBuilder<> IRB(pi) ; 
            
            // call llvm.returnaddress ( a codegen intrinsic )
            Constant* zero = IRB.getInt32(0x0) ;
            CallInst* get_retaddr = IRB.CreateCall(func_retaddr, zero);
            
            // DEBUG : call printf("%p\n", p) ; 
            if ( debug ) {
                Value* fstr = IRB.CreateGlobalStringPtr("retaddr : %p\n") ;
                std::vector<Value *> args_printf({fstr, get_retaddr}) ;
                IRB.CreateCall(func_printf, args_printf);
            }

            // __shadow_stack_ptr -= sizeof(void *) ; // stack grows backward
            LoadInst* ssp = IRB.CreateLoad(shadow_sp) ;
            Constant* neg_eight = IRB.getInt64(-0x8 / 8) ; 
            Value* gep = IRB.CreateInBoundsGEP(ssp, neg_eight) ;
            IRB.CreateStore(gep, shadow_sp) ;
            
            // *__shadow_stack_ptr = <return address> ; 
            LoadInst* updated_ssp = IRB.CreateLoad(shadow_sp) ;
            IRB.CreateStore(get_retaddr, updated_ssp); 
            
            // DEBUG 
            if ( debug ) {
                Value* dfstr = IRB.CreateGlobalStringPtr("%p: %p has store\n") ;
                LoadInst* load_ssp  = IRB.CreateLoad(shadow_sp) ;
                LoadInst* load_cssp = IRB.CreateLoad(load_ssp) ;
                std::vector<Value *> v({dfstr, load_ssp, load_cssp}) ;
                IRB.CreateCall(func_printf, v) ;
            }
        }
        
        // function epilogue
        void createShadowStackEpilogue(Instruction *pi) {

            Module *M = pi->getModule() ;
            BasicBlock* BB = pi->getParent() ;
            
            BasicBlock* BB_chk  = BB->splitBasicBlock(pi) ;
            BasicBlock* BB_exit = BB_chk->splitBasicBlock(pi) ;
            BasicBlock* BB_ret  = BB_exit->splitBasicBlock(pi) ;
            
            // functions
            FunctionCallee func_printf = M->getFunction("printf")  ;
            FunctionCallee func_retaddr= M->getFunction("llvm.returnaddress") ;
            FunctionCallee func_exit   = M->getFunction("exit");
            
            /***********************
             *** IRB_chk
             ***********************/
            IRBuilder<> IRB_chk(BB_chk->getTerminator()) ;
            Constant* i32_zero    = IRB_chk.getInt32(0) ;
            CallInst* get_retaddr = IRB_chk.CreateCall(func_retaddr, i32_zero); 
            LoadInst* load_shadow_sp   = IRB_chk.CreateLoad(shadow_sp);
            LoadInst* load_shadow_addr = IRB_chk.CreateLoad(load_shadow_sp) ;
            Value* cmp_retaddr = IRB_chk.CreateICmpEQ(get_retaddr, load_shadow_addr) ;
            IRB_chk.CreateCondBr(cmp_retaddr, BB_ret, BB_exit) ;
            BB_chk->getTerminator()->eraseFromParent() ;
            
            /***********************
             *** IRB_exit
             ***********************/
            IRBuilder<> IRB_exit(BB_exit->getTerminator()) ;
            Value* error_msg = IRB_exit.CreateGlobalStringPtr("[ERROR] Return address compromised !!!!\n") ;
            IRB_exit.CreateCall(func_printf, error_msg);
            
            Constant* i32_neg_one = IRB_exit.getInt32(-1) ;
            IRB_exit.CreateCall(func_exit, i32_neg_one) ; 
            IRB_exit.CreateUnreachable() ;
            BB_exit->getTerminator()->eraseFromParent() ;
            
            
            /***********************
             *** IRB_ret
             ***********************/
            IRBuilder<> IRB_ret(BB_ret->getTerminator()) ;
            LoadInst* load_ssp = IRB_ret.CreateLoad(shadow_sp) ;
            Constant* eight    = IRB_ret.getInt64(0x8 / 8);
            Value* gep = IRB_ret.CreateInBoundsGEP(load_ssp, eight);
            IRB_ret.CreateStore(gep, shadow_sp) ;  
        
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

static RegisterPass<ShadowStackPass> X("shadowStackPass", "Shadow Stack Pass",
                                        false, false) ; 

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

