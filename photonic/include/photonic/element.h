#pragma once
// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: PhotonicElement — Base Interface for Photonic Processing
//  Every optical component (activation, MZI, splitter, etc.) implements
//  this interface. This is the photonic equivalent of a logic gate.
// ═══════════════════════════════════════════════════════════════════════

#include "core/wave.h"
#include <string>
#include <memory>
#include <map>
#include <functional>

namespace phx {

// ═════════════════════════════════════════════════════════════════════
//  PhotonicElement — Single-Channel Optical Processor
// ═════════════════════════════════════════════════════════════════════
// Processes one Wave → one Wave.
// Equivalent to a GPU instruction operating on a single value.
//
// Examples:
//   - Saturable Absorber (ReLU)     — intensity thresholding
//   - MZI Modulator (Sigmoid)       — interference-based S-curve
//   - Bistable Cavity (Tanh)        — phase-flip non-linearity
//   - Phase Shifter                 — pure phase rotation (future)
//   - Beam Splitter                 — 50/50 power division (future)

class PhotonicElement {
public:
    virtual ~PhotonicElement() = default;

    // Process a single wave through this optical element
    virtual Wave process(const Wave& input) const = 0;

    // Human-readable name of the element
    virtual std::string name() const = 0;

    // Physical description of the element
    virtual std::string description() const = 0;

    // Print element info
    friend std::ostream& operator<<(std::ostream& os, const PhotonicElement& el) {
        os << "[" << el.name() << "] " << el.description();
        return os;
    }
};


// ═════════════════════════════════════════════════════════════════════
//  MultiChannelElement — Multi-Channel Optical Processor
// ═════════════════════════════════════════════════════════════════════
// Processes WaveChannel → WaveChannel (N waves simultaneously).
// Required for operations that need global context across channels.
//
// The key distinction: a PhotonicElement processes each wave
// independently, while a MultiChannelElement has cross-channel
// coupling (like gain competition in a shared laser cavity).
//
// Examples:
//   - Cross-Gain Modulation (Softmax) — gain competition
//   - MZI Mesh (future)               — unitary matrix transform

class MultiChannelElement {
public:
    virtual ~MultiChannelElement() = default;

    // Process all channels simultaneously
    virtual WaveChannel process(const WaveChannel& inputs) const = 0;

    // Human-readable name
    virtual std::string name() const = 0;

    // Physical description
    virtual std::string description() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const MultiChannelElement& el) {
        os << "[" << el.name() << "] " << el.description();
        return os;
    }
};


// ═════════════════════════════════════════════════════════════════════
//  Factory — Create activations by name
// ═════════════════════════════════════════════════════════════════════

using ParamMap = std::map<std::string, double>;

// Factory function declarations (defined in respective .cpp files)
std::unique_ptr<PhotonicElement> create_relu_sa(const ParamMap& params = {});
std::unique_ptr<PhotonicElement> create_sigmoid_mzi(const ParamMap& params = {});
std::unique_ptr<PhotonicElement> create_tanh_bistable(const ParamMap& params = {});
std::unique_ptr<MultiChannelElement> create_softmax_xgm(const ParamMap& params = {});

// Create a single-channel activation by name
std::unique_ptr<PhotonicElement> create_activation(
    const std::string& name,
    const ParamMap& params = {}
);

}  // namespace phx
