#!/usr/bin/python3
import os
pwd = "."
def error(msg) :
    print("Error: " + msg)
    quit()

def build_llvm() :

    llvm_root = os.path.abspath("./llvm")
    llvm_build= llvm_root + "/build"
    clang = llvm_build + "/bin/clang"
    if os.path.exists(clang) :
        return

    if not os.path.exists(llvm_root) :
        error("Please change directory to the root of repo")
    if not os.path.exists(llvm_build) :
        os.mkdir(llvm_build)

    pwd = os.path.abspath(os.getcwd())
    ls = os.listdir(llvm_build)
    os.chdir(llvm_build)

    # cmake
    if len(ls) == 0 :
        os.system('cmake ..')

    # make
    os.system('make')
    os.chdir(pwd)

def main() :
    build_llvm()
    print("Done !")

if __name__ == '__main__' :
    main()
