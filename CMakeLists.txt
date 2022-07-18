cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=c99 -Wall -Wextra -pedantic -pipe")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
	message("Release build")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -flto")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	message("Debug build")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
endif()

if (${CMAKE_SYSTEM_NAME} NOT STREQUAL "Windows")
	# Use mold on Linux
	set(CMAKE_C_COMPILER clang)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=mold")
endif()

project(main)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(raylib 4.0.0 QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
	include(FetchContent)
		FetchContent_Declare(
		raylib
		URL https://github.com/raysan5/raylib/archive/refs/tags/4.0.0.tar.gz
		URL_HASH SHA256=11f6087dc7bedf9efb3f69c0c872f637e421d914e5ecea99bbe7781f173dc38c
	)
	FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set(IS_WINDOWS "-windows")
	set(WASMER_LIB "${CMAKE_CURRENT_SOURCE_DIR}/deps/wasmer/package/lib/wasmer.dll.a")
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(WASMER_LIB "${CMAKE_CURRENT_SOURCE_DIR}/deps/wasmer/package/lib/libwasmer.a")

endif()


add_custom_command(
	OUTPUT
		${CMAKE_CURRENT_SOURCE_DIR}/deps/wasmer/package/
		${WASMER_LIB}
		COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/deps/wasmer/ build-capi${IS_WINDOWS} 
		COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/deps/wasmer/ package-capi 
		COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/deps/wasmer/package/lib/wasmer.dll . || true
)

add_custom_target(
	wasmer ALL
	DEPENDS ${WASMER_LIB}
)

if(NOT WASMER_LIB)
    message(FATAL_ERROR "wasmer library not found")
endif()

if(WASMER_LIB)
	message("Wasmer lib located at ${WASMER_LIB}")
endif()


file(GLOB SRC_FILES src/*.c)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} raylib ${WASMER_LIB})

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
