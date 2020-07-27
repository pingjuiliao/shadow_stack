#include "llvm/Pass.h"
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
        GlobalVariable* shadow_bound ;
        GlobalVariable* shadow_sp ;
        ShadowStackPass() : FunctionPass(ID) {}
        
        virtual bool doInitialization(Module &M) {
            LLVMContext& C = M.getContext() ;
            Constant* null = ConstantPointerNull::get(PointerType::getInt8PtrTy(C)) ;
            shadow_sp = new GlobalVariable(M, PointerType::getInt64PtrTy(C), false, GlobalValue::CommonLinkage, null, "__shadow_stack_pointer");
            shadow_bound = new GlobalVariable(M, PointerType::getInt64PtrTy(C), false, GlobalValue::CommonLinkage, null, "__shadow_stack_base") ;
            
        }

        // Allocate shadow stack memory 
        void createAllocationPrologue(Function &F) {
            BasicBlock &B   = F.getEntryBlock() ;
            Instruction *pi = B.getFirstNonPHI() ; 
            
            LLVMContext& C = pi->getContext() ;
            Module *M = pi->getModule() ;

            
            Value* func_mmap   = M->getOrInsertFunction("mmap", PointerType::getInt8PtrTy(C));
            Value* func_printf = M->getOrInsertFunction("printf", IntegerType::getInt32Ty(C)); 
            Value* func_puts   = M->getOrInsertFunction("puts", Type::getVoidTy(C)) ;
            Value* func_memset = M->getOrInsertFunction("memset", Type::getVoidTy(C)) ;
            IRBuilder<> IRB(pi) ; 
            Constant* null      = ConstantPointerNull::get(PointerType::getInt8PtrTy(C)) ;
            // *buf = mmap(...)
            std::vector<Value*> arg_mmap ;
            arg_mmap.emplace_back(null)  ; // random address
            arg_mmap.emplace_back(ConstantInt::get(IntegerType::getInt64Ty(C), 0x1000)); // size : 0x1000
            arg_mmap.emplace_back(ConstantInt::get(IntegerType::getInt32Ty(C), 0x3))   ; // exec : PROT_READ | PROT_WRITE
            arg_mmap.emplace_back(ConstantInt::get(IntegerType::getInt32Ty(C), 0x22))  ; // opt  : MAP_PRIVATE | MAP_ANONYMOUS
            arg_mmap.emplace_back(ConstantInt::get(IntegerType::getInt32Ty(C), 0xffffffff)) ; // fd = -1
            arg_mmap.emplace_back(null)  ; // some structure
            CallInst* call_mmap = IRB.CreateCall(func_mmap, arg_mmap);
            
            // STORE shadow stack bound
            // TODO : hide this (instead of using global variable)
            StoreInst* store_bound = IRB.CreateStore(call_mmap, shadow_bound);
            
            // STORE shadow stack base
            Constant* base_shift   = ConstantInt::get(IntegerType::getInt64Ty(C), 0x1000 - 8) ;
            Value* shadow_base     = IRB.CreateInBoundsGEP(call_mmap, base_shift);
            StoreInst* store_ssp   = IRB.CreateStore(shadow_base, shadow_sp);

            
            std::vector<Value*> arg_printf ;
            Value* bb_fstr =  IRB.CreateGlobalStringPtr("Bound: %p, Base: %p\n") ; 
            arg_printf.emplace_back(bb_fstr) ;
            LoadInst* ss_bound = IRB.CreateLoad(shadow_bound);
            arg_printf.emplace_back(ss_bound) ;
            LoadInst* ss_base  = IRB.CreateLoad(shadow_sp) ;
            arg_printf.emplace_back(ss_base) ;
            CallInst* printf_ssbb = IRB.CreateCall(func_printf, arg_printf) ;
        }
        
        // function prologue
        void createShadowStackPrologue(Function &F) {
            // 
            BasicBlock &B   = F.getEntryBlock() ;
            Instruction *pi = B.getFirstNonPHI() ; 
            LLVMContext& C = pi->getContext() ;
            Module *M = pi->getModule() ;
            Value* func_printf = M->getOrInsertFunction("printf", IntegerType::getInt32Ty(C)); 
           
            Constant* null = ConstantPointerNull::get(PointerType::getInt8PtrTy(C)) ;
            // Value* func_retaddr = M->getOrInsertFunction("returnaddress", IntegerType::getInt32Ty(C)); 
            if ( F.hasPrefixData() ) {
                Constant *d = F.getPrefixData() ;
                errs() << "hasPrefixData!!\n"  ;
            }
            if ( F.hasPrologueData() ) { 
                Constant *e = F.getPrologueData() ;
                errs() << "hasPrologueData!!\n" ;
            }
            AttributeList att_list = F.getAttributes();  
            
            IRBuilder<> IRB(pi) ;
            
            // call llvm.returnaddress ( a codegen intrinsic )
            Value* func_retaddr = M->getOrInsertFunction("llvm.returnaddress", PointerType::getInt8PtrTy(C));   
            Constant* zero = ConstantInt::get(IntegerType::getInt32Ty(C), 0x0);
            CallInst* get_retaddr = IRB.CreateCall(func_retaddr, zero);
            
            // call printf("%p\n", p) ; 
            /*
            std::vector<Value*> arg_printf ;
            Value* retaddr_fstr = IRB.CreateGlobalStringPtr("llvm.retaddr: %p\n");
            arg_printf.emplace_back(retaddr_fstr);
            arg_printf.emplace_back(get_retaddr) ;
            CallInst* call_printf = IRB.CreateCall(func_printf, arg_printf) ;
            */
            
            // __shadow_stack_ptr -= sizeof(void *) ; // stack grows backward
            LoadInst* ssp = IRB.CreateLoad(shadow_sp) ;
            Constant* neg_eight = 
                ConstantInt::get(IntegerType::getInt64Ty(C), -0x8) ; 
            Value* gep = IRB.CreateInBoundsGEP(ssp, neg_eight) ;
            StoreInst* shadow_push = IRB.CreateStore(gep, shadow_sp) ;

            // *__shadow_stack_ptr = <return address> ; 
            LoadInst* updated_ssp = IRB.CreateLoad(shadow_sp) ;
            StoreInst* store_retaddr = IRB.CreateStore(get_retaddr, updated_ssp); 
            /* Shadow Stack  
            std::vector<Value*> arg_shadow_printf ;
            Value* shadow_fstr = IRB.CreateGlobalStringPtr("Shadow push <%p> : %p\n");
            arg_shadow_printf.emplace_back(shadow_fstr) ;
            LoadInst* load_ssp = IRB.CreateLoad(shadow_sp) ;
            arg_shadow_printf.emplace_back(load_ssp) ;
            LoadInst* load_cssp= IRB.CreateLoad(load_ssp) ;
            arg_shadow_printf.emplace_back(load_cssp) ;
            CallInst* shadow_printf = IRB.CreateCall(func_printf, arg_shadow_printf) ;
            */
        }
        
        // function epilogue
        void createShadowStackEpilogue(Instruction *pi) {

            LLVMContext& C = pi->getContext() ;
            Module *M = pi->getModule() ;
            BasicBlock* BB = pi->getParent() ;
            
            BasicBlock* BB_chk  = BB->splitBasicBlock(pi) ;
            BasicBlock* BB_exit = BB_chk->splitBasicBlock(pi) ;
            BasicBlock* BB_ret  = BB_exit->splitBasicBlock(pi) ;
            
            Value* func_printf = M->getOrInsertFunction("printf", IntegerType::getInt32Ty(C)); 
            Value* func_retaddr= M->getOrInsertFunction("llvm.returnaddress", PointerType::getInt8PtrTy(C)) ;
            Value* func_exit   = M->getOrInsertFunction("exit", Type::getVoidTy(C)) ;
            Constant* null = ConstantPointerNull::get(PointerType::getInt8PtrTy(C)) ;
            ConstantInt* zero = ConstantInt::get(IntegerType::getInt64Ty(C), 0x0) ;
            // IRB_chk
            IRBuilder<> IRB_chk(BB_chk->getTerminator()) ;
            // Value* fstr = IRB_chk.CreateGlobalStringPtr("Will !!!!!!!!!\n") ;
            // CallInst* call_printf= IRB_chk.CreateCall(func_printf, fstr) ;
            CallInst* get_retaddr= IRB_chk.CreateCall(func_retaddr, zero);
            Value*    p64_retaddr= IRB_chk.CreateBitCast(get_retaddr, IntegerType::getInt64Ty(C));
            LoadInst* load_shadow_sp   = IRB_chk.CreateLoad(shadow_sp);
            LoadInst* load_shadow_addr = IRB_chk.CreateLoad(load_shadow_sp) ;
            Value* cmp_retaddr = IRB_chk.CreateICmpEQ(p64_retaddr, load_shadow_addr) ;
            BranchInst* br_retaddr = IRB_chk.CreateCondBr(cmp_retaddr, BB_ret, BB_exit) ;
            BB_chk->getTerminator()->eraseFromParent() ;

            // IRB_exit
            IRBuilder<> IRB_exit(BB_exit->getTerminator()) ;
            Value* error_msg = IRB_exit.CreateGlobalStringPtr("[ERROR] Return address compromised !!!!\n") ;
            CallInst* print_error_msg = IRB_exit.CreateCall(func_printf, error_msg);
            Constant* neg_one = ConstantInt::get(IntegerType::getInt64Ty(C), -1);
            CallInst* exit    = IRB_exit.CreateCall(func_exit, neg_one) ; 
            UnreachableInst* unreach = IRB_exit.CreateUnreachable() ;
            BB_exit->getTerminator()->eraseFromParent() ;

            // IRB_ret
            IRBuilder<> IRB_ret(BB_ret->getTerminator()) ;
            // Value* ret_fstr = IRB_ret.CreateGlobalStringPtr("Return !!!!!!!!!\n") ;
            // CallInst* call_printf2= IRB_ret.CreateCall(func_printf, ret_fstr) ;
            LoadInst* load_ssp = IRB_ret.CreateLoad(shadow_sp) ;
            Constant* eight    = ConstantInt::get(IntegerType::getInt64Ty(C), 0x8) ;
            Value* gep = IRB_ret.CreateInBoundsGEP(load_ssp, eight);
            StoreInst* store_ssp = IRB_ret.CreateStore(gep, shadow_sp) ;  

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
    };
}

char ShadowStackPass::ID = 0;

static void
registerShadowStackPass(const PassManagerBuilder &, legacy::PassManagerBase &PM) {
    PM.add(new ShadowStackPass());
}
static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerShadowStackPass);
