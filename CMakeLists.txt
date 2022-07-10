cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=c99 -Wall -Wextra -pipe")

if(CMAKE_BUILD_TYPE MATCHES RELEASE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -flto")
endif()

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
endif()

if (UNIX)
	# Use mold on Linux
	set(CMAKE_C_COMPILER clang)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fuse-ld=mold")
endif()

project(main)

conan_basic_setup()

set(OpenGL_GL_PREFERENCE GLVND)

file(GLOB SRC_FILES src/*.c)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
