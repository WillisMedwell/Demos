# make: http://gnuwin32.sourceforge.net/packages/make.htm
# clang for windows: https://solarianprogrammer.com/2021/06/11/install-clang-windows-msys2-mingw-w64/#msys2_clang_gdb_installation 
# wasm-opt: https://github.com/WebAssembly/binaryen/releases

SRC := src/index.c src/wath.c src/winearAlgebra.c src/render.c
INC := -I include
#CLANG_CFLAGS := clang -Oz -fno-builtin -Wall -Wextra --target=wasm32 -nostdinc -ffast-math -std=c18 $(INC) 
CLANG_CFLAGS := clang -Oz  -Wall -Wextra --target=wasm32 -ffast-math -std=c18 $(INC) 

WASM_OPT_CFLAGS := -Oz
all:
	$(CLANG_CFLAGS) -o intermediate/wath.o          -c src/wath.c
	$(CLANG_CFLAGS) -o intermediate/winearAlgebra.o -c src/winearAlgebra.c
	$(CLANG_CFLAGS) -o intermediate/index.o         -c src/index.c
	$(CLANG_CFLAGS) -o intermediate/render.o        -c src/render.c
	wasm-ld --no-entry --export-all -o raytracer.wasm intermediate/index.o intermediate/wath.o intermediate/winearAlgebra.o intermediate/render.o
	wasm-opt $(WASM_OPT_CFLAGS) raytracer.wasm -o raytracer.wasm

test:
	gcc -O0 -std=c18 -I include $(SRC) src/test.c -o intermediate/test.exe 
	./intermediate/test.exe