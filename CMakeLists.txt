cmake_minimum_required(VERSION 3.20)
project(phx-ptc
    VERSION 0.1.0
    DESCRIPTION "Photonic Computing Platform — Wave-Based AI Accelerator Simulator"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── MSVC-friendly flags ──────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:preprocessor)
    add_compile_definitions(_USE_MATH_DEFINES)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

enable_testing()

# ── Sub-modules ──────────────────────────────────────────────────────
add_subdirectory(core)
add_subdirectory(photonic)
add_subdirectory(ONN)
add_subdirectory(runtime)
add_subdirectory(tests)
add_subdirectory(examples)
