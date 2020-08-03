; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt %s -instcombine -S | FileCheck %s

; For pattern ((X l>> Y) & ~C) ==/!= 0; when C+1 is power of 2
; it may be optimal to fold into (X l>> Y) </>= C+1
; rather than X & (~C << Y) ==/!= 0

; Scalar tests

define i1 @scalar_i8_lshr_and_negC_eq(i8 %x, i8 %y) {
; CHECK-LABEL: @scalar_i8_lshr_and_negC_eq(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr i8 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[R:%.*]] = icmp ult i8 [[LSHR]], 4
; CHECK-NEXT:    ret i1 [[R]]
;
  %lshr = lshr i8 %x, %y
  %and = and i8 %lshr, 252  ; ~3
  %r = icmp eq i8 %and, 0
  ret i1 %r
}

define i1 @scalar_i16_lshr_and_negC_eq(i16 %x, i16 %y) {
; CHECK-LABEL: @scalar_i16_lshr_and_negC_eq(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr i16 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[R:%.*]] = icmp ult i16 [[LSHR]], 128
; CHECK-NEXT:    ret i1 [[R]]
;
  %lshr = lshr i16 %x, %y
  %and = and i16 %lshr, 65408  ; ~127
  %r = icmp eq i16 %and, 0
  ret i1 %r
}

define i1 @scalar_i32_lshr_and_negC_eq(i32 %x, i32 %y) {
; CHECK-LABEL: @scalar_i32_lshr_and_negC_eq(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[R:%.*]] = icmp ult i32 [[LSHR]], 262144
; CHECK-NEXT:    ret i1 [[R]]
;
  %lshr = lshr i32 %x, %y
  %and = and i32 %lshr, 4294705152  ; ~262143
  %r = icmp eq i32 %and, 0
  ret i1 %r
}

define i1 @scalar_i64_lshr_and_negC_eq(i64 %x, i64 %y) {
; CHECK-LABEL: @scalar_i64_lshr_and_negC_eq(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr i64 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[R:%.*]] = icmp ult i64 [[LSHR]], 8589934592
; CHECK-NEXT:    ret i1 [[R]]
;
  %lshr = lshr i64 %x, %y
  %and = and i64 %lshr, 18446744065119617024  ; ~8589934591
  %r = icmp eq i64 %and, 0
  ret i1 %r
}

define i1 @scalar_i32_lshr_and_negC_ne(i32 %x, i32 %y) {
; CHECK-LABEL: @scalar_i32_lshr_and_negC_ne(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[R:%.*]] = icmp ugt i32 [[LSHR]], 262143
; CHECK-NEXT:    ret i1 [[R]]
;
  %lshr = lshr i32 %x, %y
  %and = and i32 %lshr, 4294705152  ; ~262143
  %r = icmp ne i32 %and, 0   ; check 'ne' predicate
  ret i1 %r
}

; Vector tests

define <4 x i1> @vec_4xi32_lshr_and_negC_eq(<4 x i32> %x, <4 x i32> %y) {
; CHECK-LABEL: @vec_4xi32_lshr_and_negC_eq(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr <4 x i32> [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[R:%.*]] = icmp ult <4 x i32> [[LSHR]], <i32 8, i32 8, i32 8, i32 8>
; CHECK-NEXT:    ret <4 x i1> [[R]]
;
  %lshr = lshr <4 x i32> %x, %y
  %and = and <4 x i32> %lshr, <i32 4294967288, i32 4294967288, i32 4294967288, i32 4294967288>  ; ~7
  %r = icmp eq <4 x i32> %and, <i32 0, i32 0, i32 0, i32 0>
  ret <4 x i1> %r
}

define <4 x i1> @vec_lshr_and_negC_eq_undef1(<4 x i32> %x, <4 x i32> %y) {
; CHECK-LABEL: @vec_lshr_and_negC_eq_undef1(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr <4 x i32> [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[AND:%.*]] = and <4 x i32> [[LSHR]], <i32 -8, i32 undef, i32 -8, i32 -8>
; CHECK-NEXT:    [[R:%.*]] = icmp eq <4 x i32> [[AND]], zeroinitializer
; CHECK-NEXT:    ret <4 x i1> [[R]]
;
  %lshr = lshr <4 x i32> %x, %y
  %and = and <4 x i32> %lshr, <i32 4294967288, i32 undef, i32 4294967288, i32 4294967288>  ; ~7
  %r = icmp eq <4 x i32> %and, <i32 0, i32 0, i32 0, i32 0>
  ret <4 x i1> %r
}

define <4 x i1> @vec_lshr_and_negC_eq_undef2(<4 x i32> %x, <4 x i32> %y) {
; CHECK-LABEL: @vec_lshr_and_negC_eq_undef2(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr <4 x i32> [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[AND:%.*]] = and <4 x i32> [[LSHR]], <i32 -8, i32 -8, i32 -8, i32 -8>
; CHECK-NEXT:    [[R:%.*]] = icmp eq <4 x i32> [[AND]], <i32 0, i32 0, i32 0, i32 undef>
; CHECK-NEXT:    ret <4 x i1> [[R]]
;
  %lshr = lshr <4 x i32> %x, %y
  %and = and <4 x i32> %lshr, <i32 4294967288, i32 4294967288, i32 4294967288, i32 4294967288>  ; ~7
  %r = icmp eq <4 x i32> %and, <i32 0, i32 0, i32 0, i32 undef>
  ret <4 x i1> %r
}

define <4 x i1> @vec_lshr_and_negC_eq_undef3(<4 x i32> %x, <4 x i32> %y) {
; CHECK-LABEL: @vec_lshr_and_negC_eq_undef3(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr <4 x i32> [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[AND:%.*]] = and <4 x i32> [[LSHR]], <i32 -8, i32 -8, i32 undef, i32 -8>
; CHECK-NEXT:    [[R:%.*]] = icmp eq <4 x i32> [[AND]], <i32 0, i32 0, i32 0, i32 undef>
; CHECK-NEXT:    ret <4 x i1> [[R]]
;
  %lshr = lshr <4 x i32> %x, %y
  %and = and <4 x i32> %lshr, <i32 4294967288, i32 4294967288, i32 undef, i32 4294967288>  ; ~7
  %r = icmp eq <4 x i32> %and, <i32 0, i32 0, i32 0, i32 undef>
  ret <4 x i1> %r
}

; Extra use

; Fold happened
define i1 @scalar_lshr_and_negC_eq_extra_use_lshr(i32 %x, i32 %y, i32 %z, i32* %p) {
; CHECK-LABEL: @scalar_lshr_and_negC_eq_extra_use_lshr(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[XOR:%.*]] = xor i32 [[LSHR]], [[Z:%.*]]
; CHECK-NEXT:    store i32 [[XOR]], i32* [[P:%.*]], align 4
; CHECK-NEXT:    [[R:%.*]] = icmp ult i32 [[LSHR]], 8
; CHECK-NEXT:    ret i1 [[R]]
;
  %lshr = lshr i32 %x, %y
  %xor = xor i32 %lshr, %z  ; extra use of lshr
  store i32 %xor, i32* %p
  %and = and i32 %lshr, 4294967288  ; ~7
  %r = icmp eq i32 %and, 0
  ret i1 %r
}

; Not fold
define i1 @scalar_lshr_and_negC_eq_extra_use_and(i32 %x, i32 %y, i32 %z, i32* %p) {
; CHECK-LABEL: @scalar_lshr_and_negC_eq_extra_use_and(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[AND:%.*]] = and i32 [[LSHR]], -8
; CHECK-NEXT:    [[MUL:%.*]] = mul i32 [[AND]], [[Z:%.*]]
; CHECK-NEXT:    store i32 [[MUL]], i32* [[P:%.*]], align 4
; CHECK-NEXT:    [[R:%.*]] = icmp eq i32 [[AND]], 0
; CHECK-NEXT:    ret i1 [[R]]
;
  %lshr = lshr i32 %x, %y
  %and = and i32 %lshr, 4294967288  ; ~7
  %mul = mul i32 %and, %z  ; extra use of and
  store i32 %mul, i32* %p
  %r = icmp eq i32 %and, 0
  ret i1 %r
}

; Not fold
define i1 @scalar_lshr_and_negC_eq_extra_use_lshr_and(i32 %x, i32 %y, i32 %z, i32* %p, i32* %q) {
; CHECK-LABEL: @scalar_lshr_and_negC_eq_extra_use_lshr_and(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[AND:%.*]] = and i32 [[LSHR]], -8
; CHECK-NEXT:    store i32 [[AND]], i32* [[P:%.*]], align 4
; CHECK-NEXT:    [[ADD:%.*]] = add i32 [[LSHR]], [[Z:%.*]]
; CHECK-NEXT:    store i32 [[ADD]], i32* [[Q:%.*]], align 4
; CHECK-NEXT:    [[R:%.*]] = icmp eq i32 [[AND]], 0
; CHECK-NEXT:    ret i1 [[R]]
;
  %lshr = lshr i32 %x, %y
  %and = and i32 %lshr, 4294967288  ; ~7
  store i32 %and, i32* %p  ; extra use of and
  %add = add i32 %lshr, %z  ; extra use of lshr
  store i32 %add, i32* %q
  %r = icmp eq i32 %and, 0
  ret i1 %r
}

; Negative tests

; X is constant

define i1 @scalar_i32_lshr_and_negC_eq_X_is_constant1(i32 %y) {
; CHECK-LABEL: @scalar_i32_lshr_and_negC_eq_X_is_constant1(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr i32 12345, [[Y:%.*]]
; CHECK-NEXT:    [[R:%.*]] = icmp ult i32 [[LSHR]], 8
; CHECK-NEXT:    ret i1 [[R]]
;
  %lshr = lshr i32 12345, %y
  %and = and i32 %lshr, 4294967288  ; ~7
  %r = icmp eq i32 %and, 0
  ret i1 %r
}

define i1 @scalar_i32_lshr_and_negC_eq_X_is_constant2(i32 %y) {
; CHECK-LABEL: @scalar_i32_lshr_and_negC_eq_X_is_constant2(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr i32 268435456, [[Y:%.*]]
; CHECK-NEXT:    [[R:%.*]] = icmp ult i32 [[LSHR]], 8
; CHECK-NEXT:    ret i1 [[R]]
;
  %lshr = lshr i32 268435456, %y
  %and = and i32 %lshr, 4294967288  ; ~7
  %r = icmp eq i32 %and, 0
  ret i1 %r
}

; Check 'slt' predicate

define i1 @scalar_i32_lshr_and_negC_slt(i32 %x, i32 %y) {
; CHECK-LABEL: @scalar_i32_lshr_and_negC_slt(
; CHECK-NEXT:    [[LSHR:%.*]] = lshr i32 [[X:%.*]], [[Y:%.*]]
; CHECK-NEXT:    [[R:%.*]] = icmp slt i32 [[LSHR]], 0
; CHECK-NEXT:    ret i1 [[R]]
;
  %lshr = lshr i32 %x, %y
  %and = and i32 %lshr, 4294967288  ; ~7
  %r = icmp slt i32 %and, 0
  ret i1 %r
}

; Compare with nonzero

define i1 @scalar_i32_lshr_and_negC_eq_nonzero(i32 %x, i32 %y) {
; CHECK-LABEL: @scalar_i32_lshr_and_negC_eq_nonzero(
; CHECK-NEXT:    ret i1 false
;
  %lshr = lshr i32 %x, %y
  %and = and i32 %lshr, 4294967288  ; ~7
  %r = icmp eq i32 %and, 1  ; should be comparing with 0
  ret i1 %r
}
