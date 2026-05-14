// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: ONN Feed Forward Network Demo
// ═══════════════════════════════════════════════════════════════════════

#include "ONN/include/ffn.h"
#include "core/wave.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

using namespace phx;

void setup_console() {
#ifdef _WIN32
    SetConsoleOutputCP(65001); // UTF-8
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

int main() {
    setup_console();

    const char* orange = "\x1b[38;2;255;185;60m";
    const char* cyan = "\x1b[36m";
    const char* reset = "\x1b[0m";

    std::cout << orange << R"(
  ┌─────────────────────────────────────────────────────────┐
  │         PHX-PTC :: OPTICAL NEURAL NETWORK DEMO          │
  │           Interactive Feed Forward Layer                │
  └─────────────────────────────────────────────────────────┘
    )" << reset << std::endl;

    FFN network;
    size_t num_channels;

    std::cout << cyan << "[?] How many optical channels? " << reset;
    std::cin >> num_channels;

    WaveChannel input_bus;
    std::cout << "\n--- STEP 1: Define Input Waves ---\n";
    for (size_t i = 0; i < num_channels; ++i) {
        double intensity, phase;
        std::cout << "  Channel [" << i << "] Intensity (0.0 to 1.0): ";
        std::cin >> intensity;
        std::cout << "  Channel [" << i << "] Phase (0 to 2*PI): ";
        std::cin >> phase;
        input_bus.push_back(Wave::from_intensity(intensity, phase));
    }
    network.set_input(input_bus);

    std::cout << "\n--- STEP 2: Define Weights (Complex Coefficients) ---\n";
    std::vector<Complex> weights;
    for (size_t i = 0; i < num_channels; ++i) {
        double real, imag;
        std::cout << "  Weight [" << i << "] Real part: ";
        std::cin >> real;
        std::cout << "  Weight [" << i << "] Imag part: ";
        std::cin >> imag;
        weights.push_back(Complex(real, imag));
    }
    network.set_weights(weights);

    std::cout << "\n--- STEP 3: Define Bias (Complex Wave) ---\n";
    std::vector<Complex> biases;
    for (size_t i = 0; i < num_channels; ++i) {
        double real, imag;
        std::cout << "  Bias [" << i << "] Real part: ";
        std::cin >> real;
        std::cout << "  Bias [" << i << "] Imag part: ";
        std::cin >> imag;
        biases.push_back(Complex(real, imag));
    }
    network.set_bias(biases);

    std::cout << "\n--- STEP 4: Choose Activation ---\n";
    std::cout << "  Options: mzi (Sigmoid), saturable_absorber (ReLU), ring_resonator (Tanh), softmax\n";
    std::string act_name;
    std::cout << "  Choice: ";
    std::cin >> act_name;
    network.set_activation(act_name);

    std::cout << "\n" << orange << "[*] Executing Photonic Forward Pass..." << reset << "\n";
    network.forward();

    std::cout << "\n--- FINAL OUTPUT ---\n";
    const auto& result = network.get_output();
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << "  ch[" << i << "] " << result[i] << "\n";
    }

    std::cout << "\n" << cyan << "[!] Simulation Complete." << reset << std::endl;

    return 0;
}
