# PHX-PTC Technical Specification: Photonic Physics & Mathematics

This document details the physical mechanisms and mathematical foundations of the **PHX-PTC** platform, specifically focusing on the mapping of deep learning primitives to optical phenomena.

---

## 1. Fundamental Wave Representation

In electronic computing, information is represented as a real-valued voltage $V$. In photonic computing, information is carried by the **Complex Amplitude** of an electromagnetic wave, denoted as $\psi$.

### Mathematics:
$$\psi = |\psi| e^{i\phi}$$

Where:
- $|\psi|$ is the **Field Magnitude**.
- $\phi$ is the **Optical Phase**.
- $I = |\psi|^2$ is the **Optical Intensity** (the measurable power).

### Advantage:
A single photonic channel carries two degrees of freedom (Magnitude and Phase), enabling coherent interference and richer data representation than binary logic.

---

## 2. ReLU $\rightarrow$ Saturable Absorber (SA)

**Photonic Mechanism:** Saturable Absorption / Optical Thresholding.

### Physics:
A Saturable Absorber is an optical component where the absorption coefficient decreases as light intensity increases. 
- **Opaque Regime:** At low intensities, the material's electrons absorb all incident photons to jump to higher energy states.
- **Bleached Regime:** Once the input intensity crosses a critical threshold ($I_{threshold}$), the material's energy states become "full" (saturated). The material becomes transparent, allowing the wave to pass.

### Wave Transformation:
$$\psi_{out} = \begin{cases} 0 & \text{if } |\psi_{in}|^2 \le I_{threshold} \\ \psi_{in} \cdot \sqrt{1 - \frac{I_{threshold}}{|\psi_{in}|^2}} & \text{if } |\psi_{in}|^2 > I_{threshold} \end{cases}$$

### Deep Learning Mapping:
This creates a "Hard Threshold" identical to the **ReLU (Rectified Linear Unit)**, where signals below a certain point are suppressed to zero.

---

## 3. Sigmoid $\rightarrow$ Mach-Zehnder Interferometer (MZI)

**Photonic Mechanism:** Electro-Optic Modulation via Interference.

### Physics:
An MZI splits light into two paths, applies a phase shift to one path, and then recombines them. The recombination causes **Interference**. 
By using an intensity-dependent phase shifter (Kerr effect) and biasing the device at its **Quadrature Point** ($\pi/4$), the output power follows a cosine-squared curve.

### Wave Transformation:
$$\psi_{out} = \psi_{in} \cdot \cos\left(\alpha |\psi_{in}|^2 + \frac{\pi}{4}\right)$$

### Deep Learning Mapping:
The resulting transmission curve provides a smooth, bounded, S-shaped response. This is the photonic equivalent of the **Sigmoid** function, mapping inputs to a bounded range $[0, 1]$ in intensity.

---

## 4. Tanh $\rightarrow$ Optical Bistability

**Photonic Mechanism:** Non-linear Cavity / Phase-Shifted Dual-Output MZI.

### Physics:
In deep learning, **Tanh** maps inputs to the range $[-1, 1]$. In optics, a "negative" value represents a **180° ($\pi$) Phase Shift**. 
By utilizing a non-linear cavity or a symmetrically driven MZI, the system can smoothly transition the output wave from being perfectly **In-Phase** (+1) to perfectly **Out-of-Phase** (-1) relative to a reference.

### Wave Transformation:
$$\psi_{out} = \psi_{in} \cdot \tanh\left(\beta |\psi_{in}|^2\right) \cdot e^{i\Delta\theta}$$

### Deep Learning Mapping:
This simulates the **Hyperbolic Tangent (Tanh)** activation, allowing the network to represent "negative" weights and signals through phase-encoding.

---

## 5. Softmax $\rightarrow$ Cross-Gain Modulation (XGM)

**Photonic Mechanism:** Shared Laser Cavity / Gain Competition.

### Physics:
Softmax is a global normalization ($ \sum e^{x_i} $). In photonics, this is achieved via **Gain Competition**. When multiple wave channels are routed into a shared optical gain medium (like a laser diode):
1. The channel with the highest initial intensity stimulates the most emission.
2. It "consumes" the shared photon pool (the gain) faster than others.
3. This exponentially suppresses weaker channels while normalizing the total output energy.

### Wave Transformation:
$$\psi_{out,j} = \psi_{in,j} \cdot \sqrt{\frac{\exp(\gamma |\psi_{in,j}|^2)}{\sum_k \exp(\gamma |\psi_{in,k}|^2)}}$$

### Deep Learning Mapping:
This provides a hardware-native **Softmax**, turning a vector of intensities into a probability distribution where the sum of intensities is normalized.

---

## Summary: The Phx-PTC Compute Paradigm

| Electronic Logic | Photonic Physics | Transformation Basis |
| :--- | :--- | :--- |
| **Logic Gate** | **Interferometer / Cavity** | Wave Interference |
| **Transistor** | **Non-linear Medium** | Intensity-dependent Refraction |
| **Signal (V)** | **Complex Field ($\psi$)** | Amplitude + Phase |
| **MatMul** | **MZI Mesh** | Unitary Transformations |
