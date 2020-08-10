cmake_minimum_required(VERSION 3.10)

# llvm 10
set(LLVM_ROOT_DIR /home/pingjui/shadow_stack/llvm)
set(LLVM_BUILD_DIR ${LLVM_ROOT_DIR}/build)

# clang 10
# set(LLVM_EXTERNAL_CLANG_SOURCE_DIR /home/pingjui/llvm-project/build/bin)
set(CMAKE_CXX_COMPILER "/home/pingjui/llvm-project/build/bin/clang++")
set(CMAKE_CC_COMPILER "/home/pingjui/llvm-project/build/bin/clang")
SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")

find_package(LLVM PATHS ${LLVM_BUILD_DIR}
             NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH )
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_INCLUDE_DIRS})
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

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
    \t/home/pingjui/llvm-project/build/bin/clang\ -Xclang\ -load\ -Xclang\ ./libLLVM${PROJECT_NAME}.so\ ${MAIN}\n)
file(APPEND ${PROJECT_BINARY_DIR}/${COMPILE_SCRIPT}
    else\n)
file(APPEND ${PROJECT_BINARY_DIR}/${COMPILE_SCRIPT}
    \t/home/pingjui/llvm-project/build/bin/clang\ -Xclang\ -load\ -Xclang\ ./libLLVM${PROJECT_NAME}.so\ \$1\n)
file(APPEND ${PROJECT_BINARY_DIR}/${COMPILE_SCRIPT}
    fi\n)


add_library( LLVM${PROJECT_NAME} MODULE
    pass.cpp
)

