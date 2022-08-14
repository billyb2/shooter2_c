configure: 
	cmake -Bbuild/debug -H. -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_SYSTEM_NAME=Linux

configure_release: 
	cmake -Bbuild/release -H. -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Linux

configure_windows:
	cmake -Bbuild/debug_win -H. -GNinja -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=win.toolchain.cmake

configure_windows_release:
	cmake -Bbuild/release_win -H. -GNinja -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=win.toolchain.cmake

build:
	ninja -Cbuild/debug -j4

build_release:
	ninja -Cbuild/release -j4

build_windows:
	ninja -Cbuild/debug_win -j4

build_windows_release:
	ninja -Cbuild/release_win -j4

run_server: build 
	build/debug/main true

run_client: build
	build/debug/main false

run_release_server: build_release 
	build/release/main true

run_release_client: build_release
	build/release/main false

run_windows_client: build_windows
	wine build/debug_win/main.exe false

run_windows_server: build_windows
	wine build/debug_win/main.exe true

run_windows_release_server: build_windows_release
	wine build/release_win/main.exe true

run_windows_release_client: build_windows_release
	wine build/release_win/main.exe false

clean: 
	rm -r build/
