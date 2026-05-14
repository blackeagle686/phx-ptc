#include "ffn.h"


using namespace phx;

FFN::FFN()
{
    input = WaveChannel();
    output = WaveChannel();
    weights = std::vector<Complex>();
    bias = std::vector<Complex>();
    activation = "";
}

FFN::~FFN(){}

void FFN::set_input(WaveChannel& input)
{
    this->input = input;
}

void FFN::set_weights(std::vector<Complex>& weights)
{
    this->weights = weights;
}

void FFN::set_bias(std::vector<Complex>& bias)
{
    this->bias = bias;
}

// using mzi by default:
// MZI ~ sigmoid
// Ring Resonator ~ tanh
// Saturable Absorber ~ ReLU
// Cross Gain Modulation ~ Softmax
void FFN::set_activation(std::string activation = "mzi")
{
    this->activation = activation;
}

void FFN::forward()
{
    // init weights & b
    // activation function:
    if (activation == "mzi") input = phx::mzi(input, weights, bias);
    else if (activation == "ring_resonator") input = phx::ring_resonator(input, weights, bias);
    else if (activation == "saturable_absorber") input = phx::saturable_absorber(input, weights, bias);
    else if (activation == "cross_gain_modulation") input = phx::cross_gain_modulation(input, weights, bias);
    else input = phx::mzi(input, weights, bias);
}

void FFN::backward()
{
    
}