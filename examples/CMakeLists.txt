# examples — photonic neural network demos
add_executable(photonic_neuron photonic_neuron.cpp)
target_link_libraries(photonic_neuron PRIVATE phx_photonic)

add_executable(onn_ffn onn_ffn.cpp)
target_link_libraries(onn_ffn PRIVATE phx_photonic phx_onn)
