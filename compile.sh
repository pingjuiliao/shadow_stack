#!/bin/sh
CLANG_ROOT=/home/pingjui/llvm-project
IR_FILE=main.bc
OBJ_FILE=main.o

./llvm/build/bin/llc --filetype=obj ${IR_FILE}
${CLANG_ROOT}/build/bin/clang ${OBJ_FILE}
