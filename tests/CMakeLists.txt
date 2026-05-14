# tests — unit test executables
set(TEST_SOURCES
    test_wave.cpp
    test_relu_sa.cpp
    test_sigmoid_mzi.cpp
    test_tanh_bistable.cpp
    test_softmax_xgm.cpp
    ffn_test.cpp
)

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} PRIVATE phx_photonic phx_onn)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
