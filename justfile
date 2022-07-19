configure: 
	cmake -Bbuild/debug -H. -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_SYSTEM_NAME=Linux

configure_release: 
	cmake -Bbuild/release -H. -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME=Linux

configure_windows:
	cmake -Bbuild/debug_win -H. -GNinja -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=win.toolchain.cmake

configure_windows_release:
	cmake -Bbuild/release_win -H. -GNinja -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=win.toolchain.cmake

copy_bot_handler:
	just -u bot_src/bot_lib/ copy

copy_bot_handler_windows:
	just -u bot_src/bot_lib/ copy_windows

copy_bot_handler_release:
	just -u bot_src/bot_lib/ copy_release

copy_bot_handler_windows_release:
	just -u bot_src/bot_lib/ copy_windows_release

build: copy_bot_handler
	ninja -Cbuild/debug -j4

build_release: copy_bot_handler_release
	ninja -Cbuild/release -j4

build_windows: copy_bot_handler_windows
	ninja -Cbuild/debug_win -j4

build_windows_release: copy_bot_handler_windows_release
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
