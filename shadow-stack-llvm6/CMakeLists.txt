cmake_minimum_required(VERSION 3.10)

find_package(LLVM REQUIRED CONFIG)
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_INCLUDE_DIRS})

## set variables
get_filename_component(PROJECT_NAME  ${PROJECT_SOURCE_DIR} NAME)
SET(MAIN main.c)
SET(MAIN_SRC ${MAIN}.in)
SET(COMPILE_SCRIPT  compile.sh)
SET(COMPILE_SCRIPT_SRC ${COMPILE_SCRIPT}.in)

## copy *.c script
configure_file (
    "${PROJECT_SOURCE_DIR}/${MAIN_SRC}"
    "${PROJECT_BINARY_DIR}/${MAIN}"
)

## copy ${COMPILE_SCRIPT} script
configure_file (
    "${PROJECT_SOURCE_DIR}/${COMPILE_SCRIPT_SRC}"
    "${PROJECT_BINARY_DIR}/${COMPILE_SCRIPT}"
)
file(APPEND ${PROJECT_BINARY_DIR}/${COMPILE_SCRIPT}
    if\ \[\[\ $1\ ==\ \"\"\ \]\]\ \;\ then\n)
file(APPEND ${PROJECT_BINARY_DIR}/${COMPILE_SCRIPT}
    \tclang\ -Xclang\ -load\ -Xclang\ ./libLLVM${PROJECT_NAME}.so\ ${MAIN}\n)
file(APPEND ${PROJECT_BINARY_DIR}/${COMPILE_SCRIPT}
    else\n)
file(APPEND ${PROJECT_BINARY_DIR}/${COMPILE_SCRIPT}
    \tclang\ -Xclang\ -load\ -Xclang\ ./libLLVM${PROJECT_NAME}.so\ \$1\n)
file(APPEND ${PROJECT_BINARY_DIR}/${COMPILE_SCRIPT}
    fi\n)


add_library( LLVM${PROJECT_NAME} MODULE
    pass.cpp
)

