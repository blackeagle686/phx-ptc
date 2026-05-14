#include "ffn.h"
#include "photonic/element.h"
#include <iostream>
#include <algorithm>

namespace phx {

FFN::FFN() : activation("mzi")
{
}

FFN::~FFN() {}

void FFN::set_input(const WaveChannel& input_val) this->input = input_val;


void FFN::set_weights(const std::vector<Complex>& weights_val) this->weights = weights_val;

void FFN::set_bias(const std::vector<Complex>& bias_val) this->bias = bias_val;


void FFN::set_activation(const std::string& activation_name) this->activation = activation_name;


void FFN::forward()
{
    if (input.empty()) return;

    // 1. Initialize weights & bias if not set
    if (weights.empty()) weights = std::vector<Complex>(input.size(), Complex(1.0, 0.0));
    if (bias.empty()) bias = std::vector<Complex>(input.size(), Complex(0.0, 0.0));

    // 2. Linear Pass: y = W * x + b
    linear_cache = WaveChannel(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        Complex weighted = input[i].amplitude() * weights[i];
        Complex biased = weighted + bias[i];
        linear_cache[i] = Wave(biased);
    }

    // 3. Non-Linear Pass: Activation
    if (activation == "softmax" || activation == "cross_gain_modulation") {
        auto xgm = create_softmax_xgm();
        output = xgm->process(linear_cache);
    } else {
        auto act = create_activation(activation);
        output = WaveChannel(linear_cache.size());
        for (size_t i = 0; i < linear_cache.size(); ++i) {
            output[i] = act->process(linear_cache[i]);
        }
    }
}

void FFN::zero_grad()
{
    grad_weights = std::vector<Complex>(weights.size(), Complex(0.0, 0.0));
    grad_bias = std::vector<Complex>(bias.size(), Complex(0.0, 0.0));
}

void FFN::backward(const WaveChannel& grad_output)
{
    if (grad_weights.empty()) zero_grad();

    const double h = 1e-7; // Step size for numerical differentiation
    auto act = create_activation(activation);

    for (size_t i = 0; i < input.size(); ++i) {
        // 1. Derivative of Activation: f'(linear_cache[i])
        // Estimate complex derivative using finite difference
        Complex z = linear_cache[i].amplitude();
        Wave out_original = act->process(Wave(z));
        Wave out_perturbed = act->process(Wave(z + Complex(h, h)));
        
        Complex f_prime = (out_perturbed.amplitude() - out_original.amplitude()) / Complex(h, h);

        // 2. Chain Rule: dL/dz = dL/dy * dy/dz
        Complex dL_dz = grad_output[i].amplitude() * f_prime;

        // 3. Weight/Bias Gradients: z = w*x + b
        // Use conjugate of input for weight gradient (complex gradient rule)
        grad_weights[i] += dL_dz * std::conj(input[i].amplitude());
        grad_bias[i] += dL_dz;
    }
}

} // namespace phx
