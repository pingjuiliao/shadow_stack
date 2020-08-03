; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=riscv32 -verify-machineinstrs < %s \
; RUN:   | FileCheck %s -check-prefix=RV32I

; TODO: lbu and lhu should be selected to avoid the unnecessary masking.

@bytes = global [5 x i8] zeroinitializer, align 1

define i32 @test_zext_i8() nounwind {
; RV32I-LABEL: test_zext_i8:
; RV32I:       # %bb.0: # %entry
; RV32I-NEXT:    lui a0, %hi(bytes)
; RV32I-NEXT:    lbu a1, %lo(bytes)(a0)
; RV32I-NEXT:    addi a2, zero, 136
; RV32I-NEXT:    bne a1, a2, .LBB0_3
; RV32I-NEXT:  # %bb.1: # %entry
; RV32I-NEXT:    addi a0, a0, %lo(bytes)
; RV32I-NEXT:    lbu a0, 1(a0)
; RV32I-NEXT:    addi a1, zero, 7
; RV32I-NEXT:    bne a0, a1, .LBB0_3
; RV32I-NEXT:  # %bb.2: # %if.end
; RV32I-NEXT:    mv a0, zero
; RV32I-NEXT:    ret
; RV32I-NEXT:  .LBB0_3: # %if.then
; RV32I-NEXT:    addi a0, zero, 1
; RV32I-NEXT:    ret
entry:
  %0 = load i8, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @bytes, i32 0, i32 0), align 1
  %cmp = icmp eq i8 %0, -120
  %1 = load i8, i8* getelementptr inbounds ([5 x i8], [5 x i8]* @bytes, i32 0, i32 1), align 1
  %cmp3 = icmp eq i8 %1, 7
  %or.cond = and i1 %cmp, %cmp3
  br i1 %or.cond, label %if.end, label %if.then

if.then:
  ret i32 1

if.end:
  ret i32 0
}

@shorts = global [5 x i16] zeroinitializer, align 2

define i32 @test_zext_i16() nounwind {
; RV32I-LABEL: test_zext_i16:
; RV32I:       # %bb.0: # %entry
; RV32I-NEXT:    lui a0, %hi(shorts)
; RV32I-NEXT:    lhu a1, %lo(shorts)(a0)
; RV32I-NEXT:    lui a2, 16
; RV32I-NEXT:    addi a2, a2, -120
; RV32I-NEXT:    bne a1, a2, .LBB1_3
; RV32I-NEXT:  # %bb.1: # %entry
; RV32I-NEXT:    addi a0, a0, %lo(shorts)
; RV32I-NEXT:    lhu a0, 2(a0)
; RV32I-NEXT:    addi a1, zero, 7
; RV32I-NEXT:    bne a0, a1, .LBB1_3
; RV32I-NEXT:  # %bb.2: # %if.end
; RV32I-NEXT:    mv a0, zero
; RV32I-NEXT:    ret
; RV32I-NEXT:  .LBB1_3: # %if.then
; RV32I-NEXT:    addi a0, zero, 1
; RV32I-NEXT:    ret
entry:
  %0 = load i16, i16* getelementptr inbounds ([5 x i16], [5 x i16]* @shorts, i32 0, i32 0), align 2
  %cmp = icmp eq i16 %0, -120
  %1 = load i16, i16* getelementptr inbounds ([5 x i16], [5 x i16]* @shorts, i32 0, i32 1), align 2
  %cmp3 = icmp eq i16 %1, 7
  %or.cond = and i1 %cmp, %cmp3
  br i1 %or.cond, label %if.end, label %if.then

if.then:
  ret i32 1

if.end:
  ret i32 0
}
