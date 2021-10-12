#!/bin/sh
SOURCE_FILE=main.c
IR_FILE=main.bc
OBJ_FILE=main.o
BIN_FILE=main.exe
## assume
/usr/bin/clang -emit-llvm -c ${SOURCE_FILE}
./llvm/build/bin/llc --filetype=obj ${IR_FILE}
/usr/bin/clang ${OBJ_FILE} -o  ${BIN_FILE}
