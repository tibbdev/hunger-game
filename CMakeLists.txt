cmake_minimum_required(VERSION 3.20)
project(hunger LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(MSVC)
add_compile_options(/arch:AVX2) #make sure SIMD optimizations take place
endif()

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/*.c")

add_subdirectory(vendor)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    ${CORE_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/inc"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
)
