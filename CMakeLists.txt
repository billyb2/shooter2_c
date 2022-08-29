cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -Wextra -pedantic -pipe -gdwarf-4")

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
		set(CMAKE_C_COMPILER gcc)

		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=mold -fdiagnostics-color")

	endif()

	if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color")
		set(WINDOWS_LIBS "-lws2_32 -luserenv -lbcrypt")
	endif()


endif()

if (NOT DEFINED CMAKE_SYSTEM_NAME)
	message(FATAL_ERROR "System name not defined")

endif()

project(main)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(raylib 4.0.0 QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
	include(FetchContent)
		FetchContent_Declare(
		raylib
		URL https://github.com/raysan5/raylib/archive/refs/tags/4.2.0.tar.gz
		URL_HASH SHA256=676217604a5830cb4aa31e0ede0e4233c942e2fc5c206691bded58ebcd82a590
	)
	FetchContent_GetProperties(raylib)
  if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
    set(FETCHCONTENT_QUIET NO)
    FetchContent_Populate(raylib)
	set(CUSTOMIZE_BUILD ON CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
	set(USE_AUDIO OFF CACHE BOOL "" FORCE) 
	set(SUPPORT_GESTURES_SYSTEM OFF CACHE BOOL "" FORCE) 
	set(SUPPORT_MOUSE_GESTURES OFF CACHE BOOL "" FORCE) 
	set(SUPPORT_VR_SIMULATOR OFF CACHE BOOL "" FORCE) 
	set(SUPPORT_DISTORTION_SHADER OFF CACHE BOOL "" FORCE) 
	set(USE_WAYLAND ON CACHE BOOL "" FORCE) 

	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		# set(USE_EXTERNAL_GLFW ON CACHE BOOL "" FORCE) 

	endif()

    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

file(GLOB SRC_FILES src/*.c)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} raylib ${WINDOWS_LIBS})

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
