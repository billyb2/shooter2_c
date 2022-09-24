configure: 
	cmake -Bbuild/debug -H. -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_SYSTEM_NAME=Linux

configure_release: 
	cmake -Bbuild/release -H. -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Linux

configure_windows:
	cmake -Bbuild/debug_win -H. -GNinja -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=win.toolchain.cmake

configure_windows_release:
	cmake -Bbuild/release_win -H. -GNinja -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=win.toolchain.cmake

build_gamemodes:
	just -f src/game_modes/justfile

build: build_gamemodes
	ninja -Cbuild/debug -j4

build_release: build_gamemodes
	ninja -Cbuild/release -j4

build_windows: build_gamemodes
	ninja -Cbuild/debug_win -j4

build_windows_release: build_gamemodes
	ninja -Cbuild/release_win -j4

run: build 
	build/debug/main 

run_release: build_release 
	build/release/main 

package: build_release
	cp build/release/main ./shooter2
	tar cvfJ shooter2_c.tar.xz shooter2 maps/ sprites/

run_windows: build_windows
	wine build/debug_win/main.exe 

run_windows_release: build_windows_release
	wine build/release_win/main.exe 

package_windows: build_release
	cp build/release_win/main.exe ./shooter2.exe
	7z a shooter2_c.zip shooter2.exe maps/ sprites/

clean: 
	rm -r build/
