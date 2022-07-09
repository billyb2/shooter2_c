install_deps:
	mkdir -p build/debug
	conan install --profile conan_linux_profile --build=raylib --build=missing . -if build/debug -of build/debug/

install_deps_release:
	mkdir -p build/release
	conan install --profile conan_linux_profile --build=raylib --build=missing . -if build/release -of build/release/

configure: 
	cmake -Bbuild/debug -H. -GNinja -DCMAKE_BUILD_TYPE=Debug

configure_release: 
	cmake -Bbuild/release -H. -GNinja -DCMAKE_BUILD_TYPE=Release

build:
	ninja -Cbuild/debug -j4

build_release:
	ninja -Cbuild/release -j4

run: build 
	build/debug/bin/main

run_release: build_release 
	build/release/bin/main

clean_debug:
	rm -r build/debug

clean_release:
	rm -r build/release

clean: clean_debug clean_release
