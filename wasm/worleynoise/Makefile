SRC := src/main.cpp
INC := -I include

CLANG_WASI_SDK_FLAGS := clang++ --target=wasm32-wasi -msimd128 -fno-exceptions -O3 -z stack-size=3500000 -Wl,--export=generate -Wl,--export=getCanvas -Wl,--no-entry -stdlib=libc++ -fexperimental-library -std=c++2b 

all:
	$(CLANG_WASI_SDK_FLAGS) $(INC) -o worleynoise.wasm $(SRC)