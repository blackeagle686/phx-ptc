#pragma once 
/*
    FFN : Feed Forward Neural Network

    In digital NNs, layers are stacked sequentially:
        Input $\psi_{in}$ → Hidden Layers $\psi_{hidden}$ → Output $\psi_{out}$
    Signal flows strictly forward (no cycles, no recurrence).

    In Photonic FFNs, the principle is the same:
        Input Wave $\psi_{in}$ → Linear Layer (MZI Mesh) $\psi_{linear}$ → Activation $\psi_{act}$ → Output Wave $\psi_{out}$
    Energy flows forward through the optical network.

*/