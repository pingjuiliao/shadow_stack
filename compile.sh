#!/bin/sh
SOURCE_FILE=main.c
IR_FILE=main.bc
OBJ_FILE=main.o
BIN_FILE=main.exe
MY_CLANG=./llvm-project-10.0.1/build/bin/clang
MY_LLC=./llvm-project-10.0.1/build/bin/llc
## assume clang exists
${MY_CLANG} -emit-llvm -c ${SOURCE_FILE}
${MY_LLC} --filetype=obj ${IR_FILE}
${MY_CLANG} ${OBJ_FILE} -o  ${BIN_FILE}

#${MY_CLANG} ${SOURCE_FILE} -o ${BIN_FILE}
