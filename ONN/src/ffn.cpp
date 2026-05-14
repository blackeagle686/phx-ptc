#include "ffn.h"
#include "core/wave.h"

FFN::FFN()
{
    input = WaveChannel();
    output = WaveChannel();
    weights = std::vector<Complex>();
    bias = std::vector<Complex>();
    activation = "";
}

FFN::~FFN()
{

}

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

void FFN::set_activation(std::string activation)
{
    this->activation = activation;
}

void FFN::forward()
{
    
}

void FFN::backward()
{
    
}