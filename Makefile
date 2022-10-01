OUT_DIR:=build/
EXEC_NAME:=main
SYSTEM_NAME:=Linux
TOOLCHAIN:=
BUILD_TYPE:=
PACKAGE_NAME:=shooter2
PACKAGE_EXEC_NAME:=$(PACKAGE_NAME)
PACKAGE_TOOL:=tar cvfJ 
PACKAGE_FILE_TYPE := .tar.xz
RUN_COMMAND:=
RELEASE_BUILD:=

ifdef RELEASE
	RELEASE_BUILD:=RELEASE=true
	BUILD_TYPE:=Release
	OUT_DIR:=$(OUT_DIR)release
else
	BUILD_TYPE:=Debug
	OUT_DIR:=$(OUT_DIR)debug
endif

ifdef WINDOWS
	OUT_DIR:=$(OUT_DIR)_win
PACKAGE_EXEC_NAME:=$(PACKAGE_EXEC_NAME).exe
	EXEC_NAME:=$(EXEC_NAME).exe
	SYSTEM_NAME:=Windows
	TOOLCHAIN:=-DCMAKE_TOOLCHAIN_FILE=win.toolchain.cmake
	PACKAGE_TOOL:=7z a 
	PACKAGE_FILE_TYPE:=.zip
	PACKAGE_NAME:=$(PACKAGE_NAME)_win
	RUN_COMMAND:=wine 
endif

run: build game_modes/%.wasm
	$(RUN_COMMAND) $(OUT_DIR)/$(EXEC_NAME)

package: build
	cp $(OUT_DIR)/$(EXEC_NAME) $(PACKAGE_EXEC_NAME)
	$(PACKAGE_TOOL) $(PACKAGE_NAME)$(PACKAGE_FILE_TYPE) game_modes/ maps/ sprites/ $(PACKAGE_EXEC_NAME)
	rm $(PACKAGE_EXEC_NAME)

game_modes/%.wasm: game_modes/%.wasm
	$(MAKE) -C src/game_modes/ $(RELEASE_BUILD)

build: configure
	ninja -C $(OUT_DIR) -j2

configure: CMakeLists.txt
	/usr/bin/cmake -B$(OUT_DIR) -H. -GNinja -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_SYSTEM_NAME=$(SYSTEM_NAME) $(TOOLCHAIN)

clean: 
	rm -rf build/
	rm -f game_modes/*
	rm -rf $(OUT_DIR)
	rm -f $(PACKAGE_NAME)$(PACKAGE_FILE_TYPE)
	rm -rf src/game_modes/build/
