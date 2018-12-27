# Learning LLVM

This repository contains a collection of small examples to help get started with LLVM.

The following instructions are tested on Ubuntu 18.04/18.10 with LLVM 6.0.0/7.0.0. Do not worry about building LLVM from source right now, you can use the following instructions on a distributor-installed LLVM/Clang also. You will need to install the following packages if not already installed.

```Bash
sudo apt install llvm llvm-runtime llvm-dev lld clang clang-tools clang-format
```

## Build

```Bash
cd llvm-hacking; mkdir build; cd build;
cmake ..; make
```

## Run

+ LLVM/Clang toolchain

  + Compile a C program

  ```Bash
  clang prog2.c
  ```

  + See Clang AST

    ```Bash
    clang -Xclang -ast-dump -fsyntax-only prog2.c
    ```

  + Generate LLVM bitcode

    ```Bash
    clang -O3 -emit-llvm prog3.c -c -o prog3.bc
    clang -O0 -S -emit-llvm hello.c
    ```

  + Convert bitcode to executable

    ```Bash
    clang prog3.bc -o prog3
    ```

  + Convert textual (assembly) IR to bitcode (binary form)

    ```Bash
    llvm-as hello.ll
    ```

  + Convert LLVM bitcode to human-readable LLVM assembly language

    ```Bash
    llvm-dis prog5.bc -o prog5.ll
    ```

  + JIT compile the LLVM IR (.ll/.bc)

    ```Bash
    lli hello.c
    lli hello.bc
    ```

  + Generate assembly code

    ```Bash
    llc hello.bc
    llc hello.ll
    ```

+ Execute pass in the shared object (legacy style)

  ```Bash
  clang -Xclang -load -Xclang ./functionpass/libFunctionPass.so ../progs/hello.c
  ./a.out
  ```

+ Execute pass in the shared object (recommended)

  ```Bash
  opt -load ./functionpass/libFunctionPass.so -iteratefuncir ../progs/prog3.bc > /dev/null
  ```

+ See help for a pass with string "iterate"

  ```Bash
  opt -load functionpass/libFunctionPass.so -help | grep "iterate"
  ```

+ Disable printing the binary `.bc` file

  ```Bash
  opt -load ./functionpass/libFunctionPass.so -mutatefunctions -disable-output ../progs/prog5.bc
  ```

+ Time the passes

  ```Bash
  opt -load ./functionpass/libFunctionPass.so -iteratefuncir -time-passes < ../progs/prog3.bc > /dev/null
  ```

+ See the order of different passes

  ```Bash
  opt -load ./functionpass/libFunctionPass.so -gvn -licm --debug-pass=Structure ../progs/prog3.bc > /dev/null
  ```

+ Execute the custom pass between two existing passes

  ```Bash
  opt -load ./functionpass/libFunctionPass.so -gvn -iteratefuncir -licm --debug-pass=Structure ../progs/prog3.bc > /dev/null
  ```

+ Visualize the CFG of a function

  ```Bash
  opt -view-cfg ../progs/prog4.ll
  ```

  ```Bash
  opt -view-cfg ../progs/prog4.ll >& /dev/null
  ```

  ```Bash
  opt -dot-cfg ../progs/prog4.bc >& /dev/null
  dot -Tpdf cfg.main.dot -o cfg.main.pdf
  ```

+ Use the LLVM Statistics framework (need to enable assertions while building LLVM)

  ```Bash
  opt -stats -load ./functionpass/libFunctionPass.so -mergereturn -instrfuncs ../progs/prog10.bc > prog10.bc
  ```

## Other Details

Build `prog11.c`:

```Bash
clang -O0 -emit-llvm ./prog11.c -c -o prog11.bc
clang prog11.bc -o prog11 -lpthread
./prog11
```

## Build LLVM Source Code

This is how you can build LLVM source. We will use LLVM 7.0.0.

```Bash
mkdir llvm-7.0.0; cd llvm-7.0.0;

wget http://releases.llvm.org/7.0.0/llvm-7.0.0.src.tar.xz
tar -xvf llvm-7.0.0.src.tar.xz
wget http://releases.llvm.org/7.0.0/cfe-7.0.0.src.tar.xz
tar -xvf cfe-7.0.0.src.tar.xz
wget http://releases.llvm.org/7.0.0/clang-tools-extra-7.0.0.src.tar.xz
tar -xvf clang-tools-extra-7.0.0.src.tar.xz
wget http://releases.llvm.org/7.0.0/compiler-rt-7.0.0.src.tar.xz
tar -xvf compiler-rt-7.0.0.src.tar.xz

mv cfe-7.0.0.src clang
mv clang-tools-extra-7.0.0.src extra
mv compiler-rt-7.0.0.src compiler-rt

mv clang llvm-7.0.0.src/tools/.
mv extra llvm-7.0.0.src/tools/clang/tools/.
mv compiler-rt llvm-7.0.0.src/projects/.

mkdir llvm-7.0.0.build llvm-7.0.0.install
cd llvm-7.0.0.build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DLLVM_BUILD_EXAMPLES=1 -DCLANG_BUILD_EXAMPLES=1 -DCMAKE_INSTALL_PREFIX=/home/swarnendu/software/llvm-7.0.0/llvm-7.0.0.install/ ../llvm-7.0.0.src
make; make install
cd ..

cd llvm-7.0.0.install
./clang --version
```

## References

This tutorial borrows ideas and examples from several excellent references. There are several other articles on the Internet that I may have referred while working on these examples, I will try to include all of those over time.

+ [Writing an LLVM Pass](https://llvm.org/docs/WritingAnLLVMPass.html)

## Feedback

The contents will probably forever be a work-in-progress, as long as LLVM remains relevant. Please provide your suggestions or pull requests to improve the contents.
