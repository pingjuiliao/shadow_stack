#!/bin/sh
CLANG_ROOT=/home/pingjui/llvm-project
SOURCE_FILE=main.c
IR_FILE=main.bc
OBJ_FILE=main.o

${CLANG_ROOT}/build/bin/clang -emit-llvm -c ${SOURCE_FILE}
./llvm/build/bin/llc --filetype=obj ${IR_FILE}
${CLANG_ROOT}/build/bin/clang ${OBJ_FILE}
