# Photonic Integrated Circuit (PIC) Components ⚡

This document outlines the physical hardware components that implement the neural network activation functions in **PHX-PTC**.

---

## 1. ReLU → Saturable Absorber (SA)

The **Saturable Absorber** is a passive nonlinear device. At low intensities, the material (often a 2D material like Graphene) absorbs photons, making the waveguide opaque. At high intensities, the material "bleaches," becoming transparent and allowing light to pass.

![Saturable Absorber for ReLU](https://ars.els-cdn.com/content/image/3-s2.0-B0123693950006692-gr8.jpg)

*   **Physical Mapping**: $I_{out} = \max(0, I_{in} - I_{threshold})$
*   **Advantage**: Zero-power passive thresholding.

---

## 2. Sigmoid → Mach-Zehnder Interferometer (MZI)

The **MZI** uses constructive and destructive interference between two waveguide arms. By shifting the phase in one arm (using a thermal or electro-optic tuner), the output intensity follows a $\cos^2$ curve, which perfectly maps to the **Sigmoid** activation function when properly biased.

![MZI for Sigmoid](https://www.researchgate.net/publication/346945922/figure/fig1/AS:983763441954816@1611558890819/a-Schematic-of-the-Mach-Zehnder-interferometer-MZI-modulator-on-LN-b-Schematic-of-the.png)

*   **Physical Mapping**: $I_{out} \propto \cos^2(\Delta\phi)$
*   **Advantage**: High-speed modulation and tunable nonlinearity.

---

## 3. Tanh → Optical Bistability (Ring Resonator)

**Optical Bistability** occurs in non-linear cavities or ring resonators. When the input intensity reaches a certain level, the phase inside the cavity flips rapidly, creating a sharp S-curve that transitions from an "off" state to an "on" state, similar to the **Tanh** function.

![Ring Resonator for Tanh](https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcTcfBs8n9X2vPq7f6AfmPttV1-J-t8upw0Jtg&s)

*   **Physical Mapping**: Sharp phase-flip transition $[-1, 1]$.
*   **Advantage**: High-contrast switching and memory effects.

---

## 4. Softmax → Cross-Gain Modulation (XGM)

In an array of **Semiconductor Optical Amplifiers (SOAs)** sharing a single gain medium (photon pool), the channels compete for available photons. The strongest input "robs" the gain from the others, effectively normalizing the output intensities across all channels—the physical equivalent of **Softmax**.

![SOA Array for Softmax](file:///c:/Users/The_Last_King/OneDrive/Documents/Projects/phx-ptc/docs/images/softmax_xgm.png)

*   **Physical Mapping**: Channel competition for shared gain.
*   **Advantage**: Massively parallel normalization at the speed of light.
