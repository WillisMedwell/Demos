SRC := src/Main.cpp
INC := -I include

LOCAL_BUILD_FLAGS := clang++ -std=c++2b -O3 -g -mtune=native
WASI_BUILD_FLAGS := clang++ --target=wasm32-wasi -O3 -ffast-math -std=c++2b -msimd128 -fno-exceptions -Wl,--export=draw -Wl,--export=update -Wl,--export=img_data -funroll-loops -DNDEBUG
WASM_OPT_BUILD := wasm-opt -O4 --fast-math --enable-simd --enable-relaxed-simd


ENABLED_WARNINGS := -Wall -Wextra -Wpedantic -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused -Wpedantic -Wconversion
DISABLED_WARNINGS := -Wno-c99-extensions -Wno-gcc-compat -Wno-redundant-consteval-if

local:
	$(LOCAL_BUILD_FLAGS) $(ENABLED_WARNINGS) $(DISABLED_WARNINGS) $(INC) -o app.exe $(SRC) 

wasi:
	$(WASI_BUILD_FLAGS) $(ENABLED_WARNINGS) $(DISABLED_WARNINGS) $(INC) -o fluidsim.wasm $(SRC)

wasm-opt:
	$(WASM_OPT_BUILD) -o fluidsim.wasm fluidsim.wasm
