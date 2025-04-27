cmake_minimum_required(VERSION 3.12)


project(LZW
		VERSION 1.0.0
		DESCRIPTION "LZW Compressor"
		HOMEPAGE_URL "lucassguedes.github.io"
		)

set(CMAKE_C_STANDARD 17) 
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "./bin")
add_compile_options(-g -Wall -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

file(GLOB SOURCES src/*.c)

add_executable(lzw ${SOURCES})

target_link_libraries(lzw m)



target_include_directories(lzw PUBLIC
						   include
)


