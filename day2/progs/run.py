import os



# problem 1
# prog = 3
# compile_bit = 'clang -o0 -emit-llvm prog{}.c -c -o problem1/prog{}.bc'.format(prog, prog)
# os.system(compile_bit)

# instrument = 'opt -load ../build/functionpass/libFunctionPass.so -opcodeCounter ./problem1/prog{}.bc > ./problem1/prog{}v2.bc'.format(prog, prog)
# os.system(instrument)

# compile_exe = 'clang problem1/prog{}.bc -o problem1/prog{}'.format(prog, prog)
# os.system(compile_exe)
# compile_exev2 = 'clang problem1/prog{}v2.bc -o problem1/prog{}v2'.format(prog, prog)
# os.system(compile_exev2)

# run = './problem1/prog{}'.format(prog)
# os.system(run)
# print('---')
# runv2 = './problem1/prog{}v2'.format(prog)
# os.system(runv2)



# problem 2
# prog = 9

# compile_bit = 'clang -o0 -emit-llvm prog{}.c -c -o problem2/prog{}.bc'.format(prog, prog)
# os.system(compile_bit)

# instrument = 'opt -load ../build/functionpass/libFunctionPass.so -countbbinloop ./problem2/prog{}.bc > ./problem2/prog{}v2.bc'.format(prog, prog)
# os.system(instrument)



# problem 3
prog = 11

compile_bit = 'clang -o0 -emit-llvm prog{}.c -c -o problem3/prog{}.bc'.format(prog, prog)
os.system(compile_bit)

instrument = 'opt -load ../build/functionpass/libFunctionPass.so -instrlocks ./problem3/prog{}.bc > ./problem3/prog{}v2.bc'.format(prog, prog)
os.system(instrument)

compile_exe = 'clang problem3/prog{}.bc -o problem3/prog{} -lpthread'.format(prog, prog)
os.system(compile_exe)
compile_exev2 = 'clang problem3/prog{}v2.bc -o problem3/prog{}v2 -lpthread'.format(prog, prog)
os.system(compile_exev2)

run = './problem3/prog{}'.format(prog)
os.system(run)
print('---')
runv2 = './problem3/prog{}v2'.format(prog)
os.system(runv2)



# problem 4
# compile_bit = 'clang -o0 -emit-llvm prog9.c -c -o problem4/prog9.bc'
# os.system(compile_bit)

# instrument = 'opt -load ../build/functionpass/libFunctionPass.so -mergereturn -instrfuncs ./problem4/prog9.bc > ./problem4/prog9v2.bc'
# os.system(instrument)

# compile_exe = 'clang problem4/prog9.bc -o problem4/prog9'
# os.system(compile_exe)
# compile_exev2 = 'clang problem4/prog9v2.bc -o problem4/prog9v2'
# os.system(compile_exev2)

# run = './problem4/prog9'
# os.system(run)
# print('---')
# runv2 = './problem4/prog9v2'
# os.system(runv2)