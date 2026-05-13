# photonic — activation engines and processing elements
add_library(phx_photonic
    src/relu_sa.cpp
    src/sigmoid_mzi.cpp
    src/tanh_bistable.cpp
    src/softmax_xgm.cpp
)

target_include_directories(phx_photonic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(phx_photonic PUBLIC phx_core)
