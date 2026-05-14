# ONN — Optical Neural Network layer implementations
add_library(phx_onn
    src/ffn.cpp
)

target_include_directories(phx_onn
    PUBLIC
        include
        ${CMAKE_SOURCE_DIR} # for core/ and photonic/
)

target_link_libraries(phx_onn
    PUBLIC
        phx_core
        phx_photonic
)
