#include "ffn.h"


using namespace phx;
using namespace std;

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
    // init weights & bias if not set:
    if (weights.empty()) 
    {
        cout << "[*] Initializing weights and bias..." << endl;
        weights = std::vector<Complex>(input.size(), Complex(1.0, 0.0));
        cout << "[*] Weights initialized with shape []"
    }
    if (bias.empty()) bias = std::vector<Complex>(input.size(), Complex(0.0, 0.0));
    
    // activation function:
    if (activation == "mzi") input = phx::mzi(input, weights, bias);
    else if (activation == "ring_resonator") input = phx::ring_resonator(input, weights, bias);
    else if (activation == "saturable_absorber") input = phx::saturable_absorber(input, weights, bias);
    else if (activation == "cross_gain_modulation") input = phx::cross_gain_modulation(input, weights, bias);
    else input = phx::mzi(input, weights, bias);

    // output = input;
}

void FFN::backward()
{
    
}