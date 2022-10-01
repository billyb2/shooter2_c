DEPS:=$(wildcard deps/*.c)
MODES:=$(wildcard *.c)
BOT_LIB_FLAGS := -O3
FLAGS:=-O1
RUST_FLAGS:=
RUST_TARGET_DIR:=debug

ifdef RELEASE
	FLAGS := -O3 -flto
	RUST_FLAGS := --release
	RUST_TARGET_DIR := release

endif

all: $(patsubst %.c,build/%.wasm,$(MODES)) $(patsubst %/Cargo.toml,build/%_rs.wasm,$(wildcard */Cargo.toml))
	cp build/*.wasm ../../game_modes/

build/%.wasm: %.c build/deps/libbot.a
	clang $(FLAGS) -std=c11 -DWASM -nostdlib --target=wasm32-unknown -Wl,--export-all,--no-entry build/deps/libbot.a $< -o build/$(*F).wasm

build/%_rs.wasm: %/Cargo.toml
	cargo build $(RUST_FLAGS) --manifest-path $<
	cp $(<D)/target/wasm32-unknown-unknown/$(RUST_TARGET_DIR)/$(<D).wasm build/$(<D)_rs.wasm

build/deps/%.o: deps/%.c
	mkdir -p build/deps/
	clang $(BOT_LIB_FLAGS) -c -std=c11 -DWASM -nostdlib --target=wasm32-unknown $< -o $@

build/deps/libbot.a: $(patsubst %.c,build/%.o,$(DEPS))
	mkdir -p build/deps/
	ar -rcs build/deps/libbot.a build/deps/*.o

clean:
	rm -rf build/
	rm -f ../../game_modes/*
