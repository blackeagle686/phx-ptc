#include "ffn.h"
#include "photonic/element.h"
#include <iostream>
#include <algorithm>

namespace phx {

FFN::FFN() : activation("mzi")
{
}

FFN::~FFN() {}

void FFN::set_input(const WaveChannel& input)
{
    this->input = input;
}

void FFN::set_weights(const std::vector<Complex>& weights)
{
    this->weights = weights;
}

void FFN::set_bias(const std::vector<Complex>& bias)
{
    this->bias = bias;
}

void FFN::set_activation(const std::string& activation)
{
    this->activation = activation;
}

void FFN::forward()
{
    if (input.empty()) return;

    // 1. Initialize weights & bias if not set
    if (weights.empty()) {
        std::cout << "[*] Initializing default weights (Identity)..." << std::endl;
        weights = std::vector<Complex>(input.size(), Complex(1.0, 0.0));
    }

    if (bias.empty()) {
        std::cout << "[*] Initializing default bias (Zero)..." << std::endl;
        bias = std::vector<Complex>(input.size(), Complex(0.0, 0.0));
    }

    // 2. Linear Pass: y = W * x + b (Coherent Superposition)
    WaveChannel linear_output(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        // Apply complex weight and add complex bias
        Complex weighted = input[i].amplitude() * weights[i];
        Complex biased = weighted + bias[i];
        linear_output[i] = Wave(biased);
    }

    // 3. Non-Linear Pass: Activation Function
    std::cout << "[*] Applying Activation: " << activation << std::endl;
    
    // Check if it's a multi-channel activation (like Softmax)
    if (activation == "softmax" || activation == "cross_gain_modulation") {
        auto xgm = create_softmax_xgm();
        output = xgm->process(linear_output);
    } else {
        // Single-channel activation (ReLU, Sigmoid, Tanh)
        auto act = create_activation(activation);
        output = WaveChannel(linear_output.size());
        for (size_t i = 0; i < linear_output.size(); ++i) {
            output[i] = act->process(linear_output[i]);
        }
    }

    std::cout << "[*] Forward complete. Total Power: " << output.total_power() << std::endl;
}


void FFN::backward()
{
    // Backpropagation through photonic circuits (Phase 2)
}

} // namespace phx
