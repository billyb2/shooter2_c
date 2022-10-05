cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
set(CC gcc)
set(CMAKE_C_COMPILER gcc)
project(main C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -Wextra -pipe -ggdb -fdebug-prefix-map=../../=./")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
	message("Release build")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -flto")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	message("Debug build")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")

endif()

if (DEFINED CMAKE_SYSTEM_NAME)
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		# Use mold on Linux
		message("Using mold linker")

		if (${CMAKE_C_COMPILER_VERSION} VERSION_GREATER_EQUAL 12.1.0)
			message("Using newer compiler!")
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=mold -fdiagnostics-color")
		else()
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -B/usr/libexec/mold -fdiagnostics-color")

		endif()

	endif()

	if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color")
		set(WINDOWS_LIBS "-lws2_32 -luserenv -lbcrypt")

		#Debug windows builds boot too slowly in Wine, so I increase the
		# optimization level
		if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
			set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O1")

		endif()
	endif()


endif()

if (NOT DEFINED CMAKE_SYSTEM_NAME)
	message(FATAL_ERROR "System name not defined")

endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(OpenGL_GL_PREFERENCE GLVND)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(USE_EXTERNAL_GLFW ON CACHE BOOL "" FORCE)
endif()

set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(USE_AUDIO OFF CACHE BOOL "" FORCE) 
set(SUPPORT_GESTURES_SYSTEM OFF CACHE BOOL "" FORCE) 
set(SUPPORT_MOUSE_GESTURES OFF CACHE BOOL "" FORCE) 
set(USE_WAYLAND ON CACHE BOOL "" FORCE)
set(SUPPORT_VR_SIMULATOR OFF CACHE BOOL "" FORCE) 
set(SUPPORT_DISTORTION_SHADER OFF CACHE BOOL "" FORCE) 
add_subdirectory(deps/raylib)

set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(DISABLE_WERROR ON CACHE BOOL "" FORCE)
set(ENABLE_RDRAND ON CACHE BOOL "" FORCE)
add_subdirectory(deps/json-c)

file(GLOB SRC_FILES src/*.c src/wasm3/*.c)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} raylib json-c ${WINDOWS_LIBS})

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
