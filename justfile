configure: 
	cmake -Bbuild/debug -H. -GNinja -DCMAKE_BUILD_TYPE=Debug

configure_release: 
	cmake -Bbuild/release -H. -GNinja -DCMAKE_BUILD_TYPE=Release

configure_windows:
	cmake -Bbuild/debug_win -H. -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=win.toolchain.cmake

configure_windows_release:
	cmake -Bbuild/release_win -H. -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=win.toolchain.cmake

build:
	ninja -Cbuild/debug -j4

build_release:
	ninja -Cbuild/release -j4

build_windows:
	ninja -Cbuild/debug_win -j4

build_windows_release:
	ninja -Cbuild/release_win -j4

run: build 
	build/debug/main

run_release: build_release 
	build/release/main

run_windows: build_windows
	wine build/debug_win/main.exe

run_windows_release: build_windows_release
	wine build/release_win/main.exe

clean: 
	rm -r build/
