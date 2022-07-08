cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=c99 -O0 -Wall -Wextra -pipe")

if (UNIX AND NOT ${PLATFORM} STREQUAL "Web")
	# Use mold on Linux
	set(CMAKE_C_COMPILER /usr/bin/clang)
	set(CMAKE_LINKER /usr/bin/mold)
	
endif()

project(main)

set(OpenGL_GL_PREFERENCE GLVND)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB SRC_FILES src/*.c)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
