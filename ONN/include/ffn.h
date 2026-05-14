#pragma once 
/*
    FFN : Feed Forward Neural Network
    in digital networks: 
        net = w1*x1 + w2*x2 + b
        y = f(net)
    where f : 
        sigmoid : f(x) = 1/(1 + exp(-x))
        tanh    : f(x) = (exp(x) - exp(-x)) / (exp(x) + exp(-x))
        relu    : f(x) = max(0, x)
        softmax : f(x) = exp(x) / sum(exp(x))
    
    output of activation function: 
        a = f(w·x + b)

    where: 
        w : weight matrix
        x : input vector
        b : bias vector
        f : activation function


    ========================================================================
    ========================================================================

    in optical Networks: 
        neron = MZI 
        MZI : 
            U(theta, phi) = [
                exp(i*phi)*cos(theta) , -sin(theta) ;
                exp(i*phi)*sin(theta) ,  cos(theta) 
            ]
        where: 
            U : unitary matrix
            theta : rotation angle
            phi : phase shift
            
    to represent a netowrk of N neurons : 
        U_total = U_N * U_{N-1} * ... * U_1
        
    where : 
        U_N , U_{N-1} , ... , U_1 are unitary matrices
    
    where : 
        U = Phase * MZI * Phase * MZI * ... * Phase * MZI
    
    U : weight matrix of the network
    Phase : phase shift matrix
    MZI : MZI matrix

    activation photonic device: 
        relu    : saturable absorber 
        sigmoid : mach-zehnder interferometer
        tanh    : ring resonator
        softmax : cross-gain modulation        
*/



#include "core/types.h"
#include "core/wave.h"
#include <vector>
#include <string>

namespace phx {

class FFN {
public:
    FFN();
    ~FFN();

    void set_input(const WaveChannel& input);
    void set_weights(const std::vector<Complex>& weights);
    void set_bias(const std::vector<Complex>& bias);
    void set_activation(const std::string& activation);
    
    void forward();
    void backward(const WaveChannel& grad_output);

    // Getters
    const WaveChannel& get_output() const { return output; }
    const std::vector<Complex>& get_grad_weights() const { return grad_weights; }
    const std::vector<Complex>& get_grad_bias() const { return grad_bias; }
    void zero_grad();

private:
    WaveChannel input;
    WaveChannel output;
    WaveChannel linear_cache; // Cache for backprop
    
    std::vector<Complex> weights;
    std::vector<Complex> bias;
    
    std::vector<Complex> grad_weights;
    std::vector<Complex> grad_bias;
    
    std::string activation;
};

} // namespace phx
