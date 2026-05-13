# runtime — CLI entry point
add_executable(phx-ptc src/main.cpp)

target_link_libraries(phx-ptc PRIVATE phx_photonic)
