cmake_minimum_required(VERSION 3.20)
project(hunger LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Fixed Runtime Library string
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<NOT:$<CONFIG:Debug>>:>")

# Compatibility Flags
set(SDL_AVX OFF CACHE BOOL "" FORCE)
set(SDL_AVX2 OFF CACHE BOOL "" FORCE)
set(SDL_SSE4_1 OFF CACHE BOOL "" FORCE)
set(SDL_SSE4_2 OFF CACHE BOOL "" FORCE)
set(SDL_LIBC ON CACHE BOOL "" FORCE)      # Tell SDL to use MSVC's libc
set(SDL_STATIC ON CACHE BOOL "" FORCE)    # Explicitly signal static build

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/src/*.c")
file(GLOB_RECURSE PLATFORM_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/platform/src/*.c")

# Disable some formats that are preventing the build from working
set(SDL_IMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDL_IMAGE_JXL OFF CACHE BOOL "" FORCE)
set(SDL_IMAGE_WEBP OFF CACHE BOOL "" FORCE)
set(SDL_IMAGE_TIDY OFF CACHE BOOL "" FORCE)

# Keep the basics on (usually ON by default)
set(SDL_IMAGE_PNG ON CACHE BOOL "" FORCE)
set(SDL_IMAGE_JPG ON CACHE BOOL "" FORCE)
set(SDL_IMAGE_BACKEND_STB ON CACHE BOOL "" FORCE)

add_subdirectory(vendor)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    ${CORE_SOURCES}
    ${PLATFORM_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/inc"
    "${CMAKE_CURRENT_SOURCE_DIR}/platform/inc"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf-static
    SDL3_image::SDL3_image-static
)
