SRC := src/main.cpp
INC := -I include

WASM_BUILD_FLAGS := clang++ --target=wasm32 -O0 -std=c++2b -fno-builtin-memcpy -fno-builtin-memset -msimd128 -funroll-loops -fno-inline -g
WASM_RELEASE_BUILD_FLAGS = clang++ --target=wasm32 -O3 -std=c++2b -fno-builtin-memcpy -fno-builtin-memset -msimd128 -funroll-loops -DNDEBUG

LOCAL_BUILD_FLAGS := clang++ -std=c++2b -g -O0  -Xlinker /STACK:33554432 -static-libsan -fsanitize=undefined -fno-inline
LOCAL_RELEASE_BUILD_FLAGS := clang++ -std=c++2b -O3 -Xlinker /STACK:33554432 -DNDEBUG -funroll-loops -ffast-math

WASI_BUILD_FLAGS := clang++ --target=wasm32-wasi -O3 -std=c++2b -msimd128 -fno-exceptions -Wl,--export=entry -Wl,--export=img_data -funroll-loops -DNDEBUG

ENABLED_WARNINGS := -Wall -Wextra -Wpedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused -Wpedantic -Wconversion
DISABLED_WARNINGS := -Wno-c99-extensions -Wno-gcc-compat -Wno-redundant-consteval-if

LIBRARY_FLAGS := -fno-exceptions -nostdinc -nostdlib 
WASM_LINKER_FLAGS :=  -Wl,--no-entry  -Wl,--initial-memory=134217728 -Wl,--export=entry  -Wl,--export=update  -Wl,--export=img_data  -Wl,--export=setHeapBase  -Wl,--export=__heap_base 
	

wasm:
	$(WASM_BUILD_FLAGS) $(LIBRARY_FLAGS) $(ENABLED_WARNINGS) $(DISABLED_WARNINGS) $(WASM_LINKER_FLAGS) $(INC) -o worleynoise.wasm $(SRC) 

wasm-r:
	$(WASM_RELEASE_BUILD_FLAGS) $(LIBRARY_FLAGS) $(ENABLED_WARNINGS) $(DISABLED_WARNINGS) $(WASM_LINKER_FLAGS) $(INC) -o worleynoise.wasm $(SRC)
	wasm-opt -O4 --fast-math --enable-simd --enable-relaxed-simd -o worleynoise.wasm worleynoise.wasm 

local:
	$(LOCAL_BUILD_FLAGS) $(ENABLED_WARNINGS) $(DISABLED_WARNINGS) $(INC) -o worleynoise.exe $(SRC)

local-release:
	$(LOCAL_RELEASE_BUILD_FLAGS) $(ENABLED_WARNINGS) $(INC) -o worleynoise.exe $(SRC)
	
wasi:
	$(WASI_BUILD_FLAGS) $(INC) -o worleynoise.wasm $(SRC)