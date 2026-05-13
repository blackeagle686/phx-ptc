// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Photonic Neuron — Single Layer Demo
// ═══════════════════════════════════════════════════════════════════════
//
//  This example demonstrates a photonic neuron:
//    y = activation(W · x + b)
//
//  Electronic:                 Photonic:
//    x = voltage vector         x = WaveChannel (complex amplitudes)
//    W = weight matrix          W = MZI mesh (complex coefficients)
//    b = bias                   b = phase bias (optical path offset)
//    activation = ReLU          activation = Saturable Absorber
//
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/relu_sa.h"
#include "photonic/sigmoid_mzi.h"
#include "photonic/tanh_bistable.h"
#include "photonic/softmax_xgm.h"
#include "core/wave.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace phx;

// Simulate complex weight application (future: MZI mesh)
static Wave apply_weight(const Wave& input, Complex weight) {
    return Wave(input.amplitude() * weight);
}

// Simulate weighted sum (coherent superposition)
static Wave weighted_sum(const WaveChannel& inputs,
                         const std::vector<Complex>& weights)
{
    Wave sum;
    for (size_t i = 0; i < inputs.size(); ++i) {
        sum += apply_weight(inputs[i], weights[i]);
    }
    return sum;
}

int main() {
    std::cout << R"(
  ═══════════════════════════════════════════════════════════════
  PHX-PTC :: Photonic Neuron Demo
  ═══════════════════════════════════════════════════════════════
  Simulating: y = activation(W · x)
  Electronic logic → Photonic wave logic
  ═══════════════════════════════════════════════════════════════
)" << std::endl;

    // ── Input: 4-channel optical signal ─────────────────────────────
    WaveChannel inputs = {
        Wave::from_intensity(1.0, 0.0),
        Wave::from_intensity(0.5, PI / 4.0),
        Wave::from_intensity(2.0, PI / 6.0),
        Wave::from_intensity(0.8, -PI / 3.0)
    };

    std::cout << "  INPUT WaveChannel (4 optical channels):\n";
    for (size_t i = 0; i < inputs.size(); ++i) {
        std::cout << "    ch[" << i << "] " << inputs[i] << "\n";
    }
    std::cout << "    Total power: " << std::fixed << std::setprecision(4)
              << inputs.total_power() << "\n\n";

    // ── Weights: complex coefficients (simulating MZI mesh output) ──
    std::vector<Complex> weights = {
        Complex(0.5, 0.3),
        Complex(-0.2, 0.8),
        Complex(0.9, -0.1),
        Complex(0.4, 0.6)
    };

    std::cout << "  WEIGHTS (complex — simulating MZI mesh):\n";
    for (size_t i = 0; i < weights.size(); ++i) {
        std::cout << "    w[" << i << "] = " << std::fixed << std::setprecision(3)
                  << weights[i].real() << " + " << weights[i].imag() << "i\n";
    }

    // ── Weighted Sum: coherent superposition ────────────────────────
    Wave z = weighted_sum(inputs, weights);
    std::cout << "\n  WEIGHTED SUM (coherent superposition):\n";
    std::cout << "    z = " << z << "\n\n";

    // ── Apply each activation and compare ───────────────────────────
    std::cout << "  ACTIVATION OUTPUTS:\n";
    std::cout << "  ┌──────────────────────┬──────────────────────────────────────────┐\n";
    std::cout << "  │  Activation          │  Output                                  │\n";
    std::cout << "  ├──────────────────────┼──────────────────────────────────────────┤\n";

    // 1. ReLU / Saturable Absorber
    SaturableAbsorber sa(0.3);
    Wave y_relu = sa.process(z);
    std::cout << "  │  ReLU (SA)           │  " << std::setw(38) << std::left
              << y_relu.to_string() << std::right << " │\n";

    // 2. Sigmoid / MZI
    MachZehnderSigmoid mzi(1.0);
    Wave y_sigmoid = mzi.process(z);
    std::cout << "  │  Sigmoid (MZI)       │  " << std::setw(38) << std::left
              << y_sigmoid.to_string() << std::right << " │\n";

    // 3. Tanh / Optical Bistable
    OpticalBistable ob(0.5);
    Wave y_tanh = ob.process(z);
    std::cout << "  │  Tanh (Bistable)     │  " << std::setw(38) << std::left
              << y_tanh.to_string() << std::right << " │\n";

    std::cout << "  └──────────────────────┴──────────────────────────────────────────┘\n\n";

    // ── Softmax across channels (multi-channel activation) ──────────
    std::cout << "  SOFTMAX / XGM (multi-channel gain competition):\n";
    CrossGainModulation xgm(2.0);
    WaveChannel softmax_out = xgm.process(inputs);
    auto probs = xgm.probabilities(inputs);

    std::cout << "  ┌─────────┬──────────┬───────────┬─────────────┐\n";
    std::cout << "  │ Channel │  I_in    │  I_out    │ Probability │\n";
    std::cout << "  ├─────────┼──────────┼───────────┼─────────────┤\n";

    for (size_t j = 0; j < inputs.size(); ++j) {
        std::cout << "  │    " << j
                  << "    │ " << std::fixed << std::setprecision(4)
                  << std::setw(7) << inputs[j].get_intensity()
                  << " │ " << std::setw(9) << softmax_out[j].get_intensity()
                  << " │   " << std::setw(7) << probs[j] << "   │\n";
    }
    std::cout << "  └─────────┴──────────┴───────────┴─────────────┘\n\n";

    std::cout << "  KEY INSIGHT:\n"
              << "  In electronic computing, a neuron outputs a SCALAR.\n"
              << "  In photonic computing, a neuron outputs a WAVE — carrying\n"
              << "  both magnitude AND phase information. This enables:\n"
              << "    - Interference-based computation\n"
              << "    - Phase-encoded information\n"
              << "    - Wavelength-multiplexed parallelism\n\n";

    return 0;
}
