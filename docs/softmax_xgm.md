# Softmax: Cross-Gain Modulation

The **Softmax** function is a vector-wide operation used to normalize outputs into a probability distribution where $\sum P_i = 1$. In PHX-PTC, this is mapped to the physics of **Gain Competition** in a shared laser medium.

## 1. Physical Mechanism
In a photonic circuit, multiple waveguides (channels) can be coupled into a single "Optical Gain Medium" (like a Semiconductor Optical Amplifier or a Laser Cavity).

### Gain Competition:
1. All channels enter the shared medium and compete for the same population of excited electrons (the gain).
2. The **Stimulated Emission** rate is proportional to the number of existing photons in a channel.
3. Therefore, the channel with the highest initial intensity will "grab" the majority of the available gain, exponentially growing at the expense of the weaker channels.
4. This results in a global "winner-take-all" or "winner-takes-most" behavior.

## 2. Mathematical Modeling
The gain competition results in a distribution that follows the Boltzmann distribution, which is mathematically identical to Softmax.

### The Softmax Wave Equation (per channel $j$):
$$\psi_{out,j} = \psi_{in,j} \cdot \sqrt{\frac{\exp(\gamma |\psi_{in,j}|^2)}{\sum_k \exp(\gamma |\psi_{in,k}|^2)}}$$

### Variable Definitions:
- **$\gamma$:** The gain competition coefficient. 
    - Low $\gamma$ results in a uniform distribution (low competition).
    - High $\gamma$ results in a hard "Argmax" (strong competition).
- **$\sum_k \exp(\dots)$:** The global normalization across all channels in the bus.

## 3. Deep Learning Mapping
In traditional GPUs, Softmax is computationally expensive because it requires a global synchronization across memory. 

In PHX-PTC, **Softmax happens at the speed of light** as a physical property of the medium. The "normalization" is enforced by the finite amount of energy available in the laser gain pool. This allows for extremely large-scale Softmax operations (thousands of channels) with near-zero latency.
