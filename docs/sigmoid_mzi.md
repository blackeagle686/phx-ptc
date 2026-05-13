# Sigmoid: Mach-Zehnder Interferometry

The **Sigmoid** activation function ($f(x) = 1 / (1 + e^{-x})$) is essential for mapping outputs to a probability range $[0, 1]$. In PHX-PTC, we achieve this through the interference physics of a **Mach-Zehnder Interferometer (MZI)**.

## 1. Physical Mechanism
An MZI consists of two 50:50 beam splitters and two waveguide arms.
1. Light enters and is split into two equal paths.
2. One path travels through a "Phase Shifter" (modulated by the input signal).
3. The paths are recombined at the second splitter.

### Coherent Interference:
The recombination results in interference that depends on the phase difference $\Delta\phi$ between the two arms.
- If $\Delta\phi = 0$, you get **Constructive Interference** (maximum output).
- If $\Delta\phi = \pi$, you get **Destructive Interference** (zero output).

## 2. Mathematical Modeling
The MZI naturally produces a cosine-squared intensity response. By biasing the device to operate at its **Quadrature Point** (the steepest part of the curve), we mimic the S-shape of a Sigmoid.

### The Sigmoid Wave Equation:
$$\psi_{out} = \psi_{in} \cdot \cos\left(\alpha |\psi_{in}|^2 + \frac{\pi}{4}\right)$$

### Variable Definitions:
- **$\alpha$:** The non-linear phase-shift coefficient. This represents the "sensitivity" of the material to intensity (e.g., the Kerr coefficient).
- **$\pi/4$:** The quadrature bias. This ensures that the operating point starts halfway between constructive and destructive interference.

## 3. Deep Learning Mapping
The intensity of the output wave $I_{out} = |\psi_{out}|^2$ follows:
$$I_{out} = I_{in} \cos^2\left(\alpha I_{in} + \frac{\pi}{4}\right)$$

This function is bounded between 0 and $I_{in}$ and exhibits the classic "S-curve" monotonicity required for Sigmoid layers. In photonic circuits, this modulation happens at the frequency of the light itself, allowing for GHz-to-THz activation speeds.
