# Tanh: Optical Bistability & Phase-Flip

The **Tanh (Hyperbolic Tangent)** activation maps inputs to a $[-1, 1]$ range. In PHX-PTC, we map this to the physics of **Optical Bistability** and **Phase-Encoding**.

## 1. Physical Mechanism
Electronic Tanh produces "negative" values. Photons do not have a negative mass or negative energy; instead, "negativity" is encoded as a **180° ($\pi$) Phase Shift**.

### Phase-Shifted Interference:
In a non-linear cavity or a dual-output MZI, the system can be driven to transition between two stable states:
1. **In-Phase (+1):** The output wave is aligned with the reference carrier.
2. **Out-of-Phase (-1):** The output wave is flipped by $\pi$ radians relative to the reference.

## 2. Mathematical Modeling
The smooth transition between these two phase states is modeled using the hyperbolic tangent of the incident intensity.

### The Tanh Wave Equation:
$$\psi_{out} = \psi_{in} \cdot \tanh\left(\beta |\psi_{in}|^2\right) \cdot e^{i\Delta\theta}$$

### Variable Definitions:
- **$\beta$:** The non-linear response steepness. High $\beta$ values create a sharper transition, similar to a "hard-tanh."
- **$e^{i\Delta\theta}$:** Represents the static phase offset of the component.
- **$\tanh(\dots)$:** The scaling factor. Note that as intensity increases, the field magnitude approaches the input magnitude, while the phase remains locked to the stable state.

## 3. Deep Learning Mapping
By utilizing the phase of the wave to represent the sign of the data, PHX-PTC can perform "subtraction" and "negative weights" natively. When a "negative" wave ($\phi = \pi$) is combined with a "positive" wave ($\phi = 0$) at a beam splitter, they subtract from each other. 

This enables the implementation of fully complex-valued neural networks that can represent inhibitory and excitatory connections just like biological neurons or advanced MLP architectures.
