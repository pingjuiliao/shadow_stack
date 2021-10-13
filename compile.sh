#!/bin/sh
SOURCE_FILE=main.c
IR_FILE=main.bc
OBJ_FILE=main.o
BIN_FILE=main.exe
MY_CLANG=./llvm-project-10.0.1/build/bin/clang
## assume clang exists
#/usr/bin/clang -emit-llvm -c ${SOURCE_FILE}
#./llvm/build/bin/llc --filetype=obj ${IR_FILE}
#/usr/bin/clang ${OBJ_FILE} -o  ${BIN_FILE}

${MY_CLANG} ${SOURCE_FILE} -o ${BIN_FILE}
