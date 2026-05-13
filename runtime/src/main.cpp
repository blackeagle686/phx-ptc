// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: CLI — Photonic Computing Platform Demo
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/relu_sa.h"
#include "photonic/sigmoid_mzi.h"
#include "photonic/tanh_bistable.h"
#include "photonic/softmax_xgm.h"
#include "core/wave.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

using namespace phx;

// ═════════════════════════════════════════════════════════════════════
//  ASCII Banner
// ═════════════════════════════════════════════════════════════════════

static void print_banner() {
    const char* gold = "\x1b[38;2;255;185;60m";
    const char* reset = "\x1b[0m";

    std::cout << gold << R"(
  ╔═══════════════════════════════════════════════════════════════════╗
  ║                                                                   ║
  ║   ██████╗ ██╗  ██╗██╗  ██╗      ██████╗ ████████╗ ██████╗         ║
  ║   ██╔══██╗██║  ██║╚██╗██╔╝      ██╔══██╗╚══██╔══╝██╔════╝         ║
  ║   ██████╔╝███████║ ╚███╔╝ █████╗██████╔╝   ██║   ██║              ║
  ║   ██╔═══╝ ██╔══██║ ██╔██╗ ╚════╝██╔═══╝    ██║   ██║              ║
  ║   ██║     ██║  ██║██╔╝ ██╗      ██║        ██║   ╚██████╗         ║
  ║   ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝      ╚═╝        ╚═╝    ╚═════╝         ║
  ║                                                                   ║
  ║          PHOTONIC COMPUTING PLATFORM  v0.1.0                      ║
  ║       Wave-Based AI Accelerator Simulator                         ║
  ║                                                                   ║
  ║   GPU Logic → Photonic Logic                                      ║
  ║   Binary Signals → Complex Wave Amplitudes                        ║
  ║   Electronic Gates → Optical Elements                             ║
  ║                                                                   ║
  ╚═══════════════════════════════════════════════════════════════════╝
)" << reset << std::endl;
}

// ═════════════════════════════════════════════════════════════════════
//  ASCII Transfer Curve Plotter
// ═════════════════════════════════════════════════════════════════════

static void plot_transfer_curve(
    const std::string& title,
    const std::string& x_label,
    const std::string& y_label,
    const std::vector<double>& x_data,
    const std::vector<double>& y_data,
    int width = 60,
    int height = 20)
{
    if (x_data.empty() || y_data.empty()) return;

    double x_min = *std::min_element(x_data.begin(), x_data.end());
    double x_max = *std::max_element(x_data.begin(), x_data.end());
    double y_min = *std::min_element(y_data.begin(), y_data.end());
    double y_max = *std::max_element(y_data.begin(), y_data.end());

    // Ensure non-zero range
    if (std::abs(y_max - y_min) < 1e-10) {
        y_min -= 0.5;
        y_max += 0.5;
    }

    // Build plot grid
    std::vector<std::string> grid(height, std::string(width, ' '));

    // Plot data points
    for (size_t i = 0; i < x_data.size(); ++i) {
        int col = static_cast<int>((x_data[i] - x_min) / (x_max - x_min) * (width - 1));
        int row = static_cast<int>((y_data[i] - y_min) / (y_max - y_min) * (height - 1));
        row = height - 1 - row;  // flip Y axis

        col = std::max(0, std::min(width - 1, col));
        row = std::max(0, std::min(height - 1, row));

        grid[row][col] = '*';
    }

    // Print
    std::cout << "\n  ┌─ " << title << " ─";
    for (int i = 0; i < width - static_cast<int>(title.size()) - 4; ++i)
        std::cout << "─";
    std::cout << "┐\n";

    for (int r = 0; r < height; ++r) {
        if (r == 0)
            std::cout << std::setw(8) << std::fixed << std::setprecision(3) << y_max;
        else if (r == height - 1)
            std::cout << std::setw(8) << std::fixed << std::setprecision(3) << y_min;
        else if (r == height / 2)
            std::cout << std::setw(8) << std::fixed << std::setprecision(3)
                      << (y_min + y_max) / 2.0;
        else
            std::cout << "        ";

        std::cout << " │" << grid[r] << "│\n";
    }

    std::cout << "         └";
    for (int i = 0; i < width; ++i) std::cout << "─";
    std::cout << "┘\n";

    std::cout << "         " << std::setw(1) << std::fixed << std::setprecision(2)
              << x_min;
    int pad = width - 10;
    for (int i = 0; i < pad; ++i) std::cout << " ";
    std::cout << std::setw(6) << x_max << "\n";

    std::cout << "         " << std::string((width - static_cast<int>(x_label.size())) / 2, ' ')
              << x_label << "\n";

    std::cout << "  " << y_label << "\n\n";
}

// ═════════════════════════════════════════════════════════════════════
//  Demo Functions
// ═════════════════════════════════════════════════════════════════════

static void demo_relu_sa() {
    std::cout << "\n"
              << "  ═══════════════════════════════════════════════════════\n"
              << "  1. ReLU → SATURABLE ABSORBER / OPTICAL THRESHOLDING\n"
              << "  ═══════════════════════════════════════════════════════\n"
              << "  Physical Device: Saturable Absorber (SA)\n"
              << "  Below I_threshold: material is OPAQUE (absorbs all photons)\n"
              << "  Above I_threshold: material BLEACHES (becomes transparent)\n\n";

    SaturableAbsorber sa(0.5);
    std::cout << "  " << sa << "\n\n";

    // Sweep input intensity
    const int N = 200;
    std::vector<double> I_in_vec, I_out_vec;

    std::cout << "  ┌──────────────────────────────────────────────────────────┐\n";
    std::cout << "  │  I_input   │  I_output  │  Phase_out │  Regime          │\n";
    std::cout << "  ├──────────────────────────────────────────────────────────┤\n";

    for (int i = 0; i <= 10; ++i) {
        double I_in = i * 0.2;
        Wave w_in = Wave::from_intensity(I_in);
        Wave w_out = sa.process(w_in);

        std::string regime = (I_in <= sa.threshold()) ? "OPAQUE" : "BLEACHED";

        std::cout << "  │  " << std::fixed << std::setprecision(3) << std::setw(7) << I_in
                  << "  │  " << std::setw(8) << w_out.get_intensity()
                  << "  │  " << std::setw(8) << w_out.phase_degrees() << "°"
                  << " │  " << std::setw(14) << regime << "  │\n";
    }

    std::cout << "  └──────────────────────────────────────────────────────────┘\n";

    // Plot transfer curve
    for (int i = 0; i < N; ++i) {
        double I = i * 3.0 / N;
        Wave w = sa.process(Wave::from_intensity(I));
        I_in_vec.push_back(I);
        I_out_vec.push_back(w.get_intensity());
    }
    plot_transfer_curve("SA Transfer: I_out vs I_in", "Input Intensity", "Output Intensity",
                        I_in_vec, I_out_vec);
}

static void demo_sigmoid_mzi() {
    std::cout << "\n"
              << "  ═══════════════════════════════════════════════════════\n"
              << "  2. SIGMOID → MACH-ZEHNDER INTERFEROMETER MODULATION\n"
              << "  ═══════════════════════════════════════════════════════\n"
              << "  Physical Device: Electro-Optic Modulator / Biased MZI\n"
              << "  cos^2 interference → S-shaped sigmoid response\n\n";

    MachZehnderSigmoid mzi(1.0);
    std::cout << "  " << mzi << "\n\n";

    const int N = 200;
    std::vector<double> I_in_vec, I_out_vec, phase_vec;

    std::cout << "  ┌────────────────────────────────────────────────────────────────┐\n";
    std::cout << "  │  I_input   │  I_output  │  Transmission │  Phase_out         │\n";
    std::cout << "  ├────────────────────────────────────────────────────────────────┤\n";

    for (int i = 0; i <= 10; ++i) {
        double I_in = i * 0.3;
        Wave w_in = Wave::from_intensity(I_in);
        Wave w_out = mzi.process(w_in);

        double T = (I_in > 1e-12) ? w_out.get_intensity() / I_in : 0.0;

        std::cout << "  │  " << std::fixed << std::setprecision(3) << std::setw(7) << I_in
                  << "  │  " << std::setw(8) << w_out.get_intensity()
                  << "  │  " << std::setw(11) << T
                  << "  │  " << std::setw(8) << w_out.phase_degrees() << "°"
                  << "         │\n";
    }

    std::cout << "  └────────────────────────────────────────────────────────────────┘\n";

    for (int i = 0; i < N; ++i) {
        double I = i * 4.0 / N;
        Wave w = mzi.process(Wave::from_intensity(I));
        I_in_vec.push_back(I);
        I_out_vec.push_back(w.get_intensity());
        phase_vec.push_back(w.phase_degrees());
    }
    plot_transfer_curve("MZI Transfer: I_out vs I_in (cos^2 response)",
                        "Input Intensity", "Output Intensity", I_in_vec, I_out_vec);
    plot_transfer_curve("MZI Phase Response",
                        "Input Intensity", "Output Phase (deg)", I_in_vec, phase_vec);
}

static void demo_tanh_bistable() {
    std::cout << "\n"
              << "  ═══════════════════════════════════════════════════════\n"
              << "  3. TANH → OPTICAL BISTABILITY / PHASE-SHIFTED MZI\n"
              << "  ═══════════════════════════════════════════════════════\n"
              << "  Physical Device: Non-linear Cavity / Dual-Output MZI\n"
              << "  Smooth transition between in-phase (+1) and out-of-phase (-1)\n\n";

    OpticalBistable ob(1.0);
    std::cout << "  " << ob << "\n\n";

    const int N = 200;
    std::vector<double> I_in_vec, tanh_resp_vec, I_out_vec;

    std::cout << "  ┌──────────────────────────────────────────────────────────────┐\n";
    std::cout << "  │  I_input  │  tanh(beta*I)  │  I_output  │  Regime          │\n";
    std::cout << "  ├──────────────────────────────────────────────────────────────┤\n";

    for (int i = 0; i <= 10; ++i) {
        double I_in = i * 0.3;
        Wave w_in = Wave::from_intensity(I_in);
        Wave w_out = ob.process(w_in);

        double tanh_val = std::tanh(ob.beta() * I_in);
        std::string regime;
        if (std::abs(tanh_val) < 0.1) regime = "DESTRUCTIVE";
        else if (tanh_val > 0.9) regime = "IN-PHASE";
        else regime = "TRANSITIONING";

        std::cout << "  │  " << std::fixed << std::setprecision(3) << std::setw(7) << I_in
                  << "  │  " << std::setw(12) << tanh_val
                  << "  │  " << std::setw(8) << w_out.get_intensity()
                  << "  │  " << std::setw(14) << regime << "  │\n";
    }

    std::cout << "  └──────────────────────────────────────────────────────────────┘\n";

    for (int i = 0; i < N; ++i) {
        double I = i * 4.0 / N;
        I_in_vec.push_back(I);
        tanh_resp_vec.push_back(std::tanh(ob.beta() * I));
        Wave w = ob.process(Wave::from_intensity(I));
        I_out_vec.push_back(w.get_intensity());
    }
    plot_transfer_curve("Tanh Response: tanh(beta*I) vs I_in",
                        "Input Intensity", "tanh(beta*I)", I_in_vec, tanh_resp_vec);
    plot_transfer_curve("Bistable Transfer: I_out vs I_in",
                        "Input Intensity", "Output Intensity", I_in_vec, I_out_vec);
}

static void demo_softmax_xgm() {
    std::cout << "\n"
              << "  ═══════════════════════════════════════════════════════\n"
              << "  4. SOFTMAX → CROSS-GAIN MODULATION (XGM)\n"
              << "  ═══════════════════════════════════════════════════════\n"
              << "  Physical Device: Laser Diode Array / Shared Gain Medium\n"
              << "  Channels compete for shared photon pool → softmax normalization\n\n";

    // Demo with different gamma values
    std::vector<double> gammas = {0.1, 1.0, 5.0, 20.0};

    // Create 4-channel input: channels have different intensities
    WaveChannel input = {
        Wave::from_intensity(1.0),
        Wave::from_intensity(2.0),
        Wave::from_intensity(0.5),
        Wave::from_intensity(3.0)
    };

    std::cout << "  Input channels:\n" << "  " << input << "\n\n";

    for (double gamma : gammas) {
        CrossGainModulation xgm(gamma);

        std::cout << "  ── gamma = " << gamma << " ──────────────────────────\n";

        WaveChannel output = xgm.process(input);
        auto probs = xgm.probabilities(input);

        std::cout << "  ┌──────────────────────────────────────────────────────┐\n";
        std::cout << "  │  Channel │  I_in  │  I_out  │  Probability │ Bar    │\n";
        std::cout << "  ├──────────────────────────────────────────────────────┤\n";

        for (size_t j = 0; j < input.size(); ++j) {
            int bar_len = static_cast<int>(probs[j] * 20);
            std::string bar(bar_len, '#');

            std::cout << "  │    " << j
                      << "    │ " << std::fixed << std::setprecision(3) << std::setw(5)
                      << input[j].get_intensity()
                      << " │ " << std::setw(7) << output[j].get_intensity()
                      << " │    " << std::setw(6) << probs[j]
                      << "  │ " << std::setw(6) << std::left << bar
                      << std::right << " │\n";
        }

        double sum_prob = 0;
        for (auto p : probs) sum_prob += p;
        std::cout << "  │         │       │         │  Σ = " << std::setw(6) << sum_prob
                  << "  │        │\n";
        std::cout << "  └──────────────────────────────────────────────────────┘\n\n";
    }
}

static void demo_compare() {
    std::cout << "\n"
              << "  ═══════════════════════════════════════════════════════════════\n"
              << "  ELECTRONIC vs PHOTONIC ACTIVATION COMPARISON\n"
              << "  ═══════════════════════════════════════════════════════════════\n\n";

    std::cout << "  ┌─────────────────┬──────────────────────┬───────────────────────────┐\n";
    std::cout << "  │  Electronic     │  Photonic Equiv.     │  Physical Device          │\n";
    std::cout << "  ├─────────────────┼──────────────────────┼───────────────────────────┤\n";
    std::cout << "  │  ReLU(x)        │  SA(psi)             │  Saturable Absorber       │\n";
    std::cout << "  │  max(0, x)      │  Threshold bleaching │  Optical Discriminator    │\n";
    std::cout << "  ├─────────────────┼──────────────────────┼───────────────────────────┤\n";
    std::cout << "  │  Sigmoid(x)     │  MZI(psi)            │  Mach-Zehnder Interf.     │\n";
    std::cout << "  │  1/(1+e^-x)     │  cos^2 modulation    │  Electro-Optic Modulator  │\n";
    std::cout << "  ├─────────────────┼──────────────────────┼───────────────────────────┤\n";
    std::cout << "  │  Tanh(x)        │  Bistable(psi)       │  Non-linear Cavity        │\n";
    std::cout << "  │  (e^x-e^-x)/..  │  Phase-flip [-1,1]   │  Dual-Output MZI          │\n";
    std::cout << "  ├─────────────────┼──────────────────────┼───────────────────────────┤\n";
    std::cout << "  │  Softmax(x_i)   │  XGM(psi_j)          │  Shared Laser Cavity      │\n";
    std::cout << "  │  e^xi/Σe^xk     │  Gain competition    │  Cross-Gain Modulation    │\n";
    std::cout << "  └─────────────────┴──────────────────────┴───────────────────────────┘\n\n";

    std::cout << "  Key Insight:\n"
              << "  ─────────────\n"
              << "  Electronic computing uses VOLTAGE (real scalar, ≥0).\n"
              << "  Photonic computing uses WAVE AMPLITUDE (complex, ψ = |ψ|·e^iφ).\n\n"
              << "  A photonic channel carries MORE information per wire:\n"
              << "    - Magnitude |ψ| → signal strength\n"
              << "    - Phase φ       → sign / interference information\n\n"
              << "  This enables operations impossible in electronics:\n"
              << "    - Constructive/destructive interference\n"
              << "    - Phase-encoded computation\n"
              << "    - Wavelength-division multiplexing (WDM)\n"
              << "    - Speed-of-light propagation\n\n";
}

// ═════════════════════════════════════════════════════════════════════
//  Main
// ═════════════════════════════════════════════════════════════════════

static void print_usage() {
    std::cout << "  Usage: phx-ptc <command>\n\n"
              << "  Commands:\n"
              << "    relu      Demo Saturable Absorber (ReLU)\n"
              << "    sigmoid   Demo MZI Modulation (Sigmoid)\n"
              << "    tanh      Demo Optical Bistability (Tanh)\n"
              << "    softmax   Demo Cross-Gain Modulation (Softmax)\n"
              << "    compare   Electronic vs Photonic comparison table\n"
              << "    all       Run all demos\n\n";
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    // Set console output codepage to UTF-8 to ensure box-drawing characters render correctly
    SetConsoleOutputCP(CP_UTF8);

    // Enable ANSI escape sequences (Virtual Terminal Processing)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
    print_banner();

    std::string command = "all";
    if (argc >= 2) {
        command = argv[1];
    }

    if (command == "relu" || command == "sa") {
        demo_relu_sa();
    } else if (command == "sigmoid" || command == "mzi") {
        demo_sigmoid_mzi();
    } else if (command == "tanh" || command == "bistable") {
        demo_tanh_bistable();
    } else if (command == "softmax" || command == "xgm") {
        demo_softmax_xgm();
    } else if (command == "compare") {
        demo_compare();
    } else if (command == "all") {
        demo_compare();
        demo_relu_sa();
        demo_sigmoid_mzi();
        demo_tanh_bistable();
        demo_softmax_xgm();
    } else {
        std::cerr << "  Unknown command: " << command << "\n\n";
        print_usage();
        return 1;
    }

    std::cout << "  ═══════════════════════════════════════════════════════\n"
              << "  PHX-PTC v0.1.0 — Phase 1 Complete\n"
              << "  Photonic Activation Functions Operational\n"
              << "  ═══════════════════════════════════════════════════════\n\n";

    return 0;
}
