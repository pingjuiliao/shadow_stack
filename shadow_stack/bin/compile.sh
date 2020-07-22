#!/bin/bash
if [[ $1 == "" ]] ; then
	clang -Xclang -load -Xclang ./libLLVMshadow_stack.so main.c
else
	clang -Xclang -load -Xclang ./libLLVMshadow_stack.so $1
fi
