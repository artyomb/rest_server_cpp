cmake_minimum_required(VERSION 3.14)
project(REST_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output_bin)

include(FetchContent)

FetchContent_Declare(
        drogon
        GIT_REPOSITORY https://github.com/drogonframework/drogon.git
        GIT_TAG v1.9.7  # Specify the Drogon version
)

set(BUILD_POSTGRESQL ON)
set(BUILD_CTL OFF)
set(BUILD_EXAMPLES OFF)
set(BUILD_YAML_CONFIG OFF)
#set(USE_STATIC_LIBS_ONLY OFF)

FetchContent_MakeAvailable(drogon)
find_package(cxxopts REQUIRED)

add_executable(rest_server src/main.cpp)

target_link_libraries(rest_server PRIVATE cxxopts::cxxopts drogon )

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()