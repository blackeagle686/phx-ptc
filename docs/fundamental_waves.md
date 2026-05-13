# Fundamental Photonic Waves

In PHX-PTC, we replace the electronic **bit** (binary voltage) with the **optical wave** (complex amplitude). This fundamental shift in data representation is what enables speed-of-light AI acceleration.

## 1. The Complex Field ($\psi$)

In traditional electronics, a signal is a real scalar value $V(t)$. In photonics, we represent a signal as a complex optical field amplitude $\psi$.

### Mathematical Representation
The wave is defined in the complex plane as:
$$\psi = |\psi| e^{i\phi} = \text{Re}(\psi) + i\text{Im}(\psi)$$

Where:
- **$|\psi|$ (Magnitude):** Represents the strength of the electric field.
- **$\phi$ (Phase):** Represents the position of the wave within its oscillation cycle.
- **$I = |\psi|^2$ (Intensity):** The measurable power of the signal. This is the square of the magnitude.

## 2. Why Waves are Better than Bits

Electronic computing is limited by binary states (0 or 1). Photonic computing leverages the continuous nature of waves to encode more information per channel.

### Coherent Superposition
When two waves meet, they don't just add their values like numbers; they interfere based on their phases:
$$\psi_{total} = \psi_1 + \psi_2$$

- **Constructive Interference:** If $\phi_1 = \phi_2$, the amplitudes add perfectly, doubling the field magnitude ($2\psi$) and quadrupling the intensity ($4I$).
- **Destructive Interference:** If $\phi_1 = \phi_2 + \pi$, the waves cancel each other out entirely ($0\psi$, $0I$).

### Information Density
By modulating both the **Intensity** and the **Phase**, a single waveguide can carry complex-valued data. This allows for native support of complex-valued neural networks, which are significantly more powerful for processing periodic or wave-like data.

## 3. Propagation Physics
Light travels at $c \approx 3 \times 10^8$ m/s. In a silicon waveguide, this speed is reduced by the refractive index $n$ ($v = c/n$), but it remains orders of magnitude faster than the drift velocity of electrons in copper wires. 

Because photons are bosons and do not have charge, they do not interact with each other in linear media. This allows for **Wavelength-Division Multiplexing (WDM)**, where multiple independent signals travel through the same waveguide simultaneously at different frequencies without crosstalk.
