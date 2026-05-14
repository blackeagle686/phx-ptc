#pragma once 
/*
    FFN : Feed Forward Neural Network

    In digital NNs, layers are stacked sequentially:
        Input $\psi_{in}$ → Hidden Layers $\psi_{hidden}$ → Output $\psi_{out}$
    Signal flows strictly forward (no cycles, no recurrence).

    In Photonic FFNs, the principle is the same:
        Input Wave $\psi_{in}$ → Linear Layer (MZI Mesh) $\psi_{linear}$ → Activation $\psi_{act}$ → Output Wave $\psi_{out}$
    Energy flows forward through the optical network.


    Mathematical Representation
    In standard deep learning, a linear layer is represented as:

    H = WX + b
    
    Where:
    - X is the input vector
    - W is the weight matrix
    - b is the bias vector
    
    In our photonic implementation, the linear transformation is achieved through the MZI mesh:
    
    $\psi_{linear} = U \cdot \psi_{in}$
    
    Where:
    - $\psi_{in}$ is the input wave (complex amplitude vector)
    - U is the unitary matrix representing the MZI mesh
    - $\psi_{linear}$ is the output of the linear layer (complex amplitude vector)
    
    The activation function is then applied element-wise:
    
    $\psi_{out} = \text{Activation}(\psi_{linear})$
    
*/