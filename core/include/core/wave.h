#pragma once
// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Wave — Fundamental Data Type of Photonic Computing
//  Replaces binary voltage signals with complex optical field amplitudes.
// ═══════════════════════════════════════════════════════════════════════

#include "core/types.h"
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace phx {

// ═════════════════════════════════════════════════════════════════════
//  Wave — Single Complex Optical Field Amplitude
// ═════════════════════════════════════════════════════════════════════
// In electronic computing: a wire carries a voltage (scalar, real).
// In photonic computing: a waveguide carries a wave (complex amplitude).
//
//   ψ = |ψ| · e^(iφ)
//
// The wave carries TWO degrees of freedom:
//   1. Magnitude |ψ| → encodes signal strength
//   2. Phase φ      → encodes sign / interference information
//
// This is fundamentally richer than binary — a single photonic
// channel carries more information than a single electronic wire.

class Wave {
public:
    // ── Constructors ────────────────────────────────────────────────

    // Default: vacuum (no light)
    Wave() : psi_(0.0, 0.0) {}

    // From raw complex amplitude
    explicit Wave(Complex amplitude) : psi_(amplitude) {}

    // From real amplitude (zero phase)
    explicit Wave(double real_amplitude) : psi_(real_amplitude, 0.0) {}

    // From magnitude and phase (polar form)
    // ψ = magnitude · e^(iφ)
    
    static Wave from_polar(double magnitude, Phase phi) {
        return Wave(std::polar(magnitude, phi));
    }

    // From intensity and phase
    // ψ = √I · e^(iφ)
    static Wave from_intensity(Intensity I, Phase phi = 0.0) {
        if (I < 0.0) throw std::invalid_argument("Intensity cannot be negative");
        return from_polar(std::sqrt(I), phi);
    }

    // Accessors 

    // Raw complex amplitude
    Complex amplitude() const { return psi_; }

    // Optical intensity (measurable power): |ψ|²
    Intensity get_intensity() const { return phx::intensity(psi_); }

    // Field magnitude: |ψ|
    double magnitude() const { return std::abs(psi_); }

    // Optical phase: arg(ψ) in radians [-π, π]
    Phase get_phase() const { return phx::phase(psi_); }

    // Phase in degrees [-180°, 180°]
    double phase_degrees() const { return to_degrees(get_phase()); }

    // Is vacuum (no light)?
    bool is_dark() const { return get_intensity() < DEFAULT_TOL; }

    // Optical Operations 

    // Attenuate: multiply field by real factor [0, 1]
    // Models optical loss, partial absorption
    Wave attenuate(double factor) const {
        return Wave(psi_ * factor);
    }

    // Phase shift: ψ → ψ · e^(iΔθ)
    // Models propagation through medium, electro-optic modulation
    Wave phase_shift(Phase delta_theta) const {
        return Wave(psi_ * std::polar(1.0, delta_theta));
    }

    // Scale by complex factor (general transformation)
    Wave scale(Complex factor) const {
        return Wave(psi_ * factor);
    }

    // Superposition (wave interference)

    Wave operator+(const Wave& other) const {
        return Wave(psi_ + other.psi_);
    }

    Wave operator-(const Wave& other) const {
        return Wave(psi_ - other.psi_);
    }

    Wave& operator+=(const Wave& other) {
        psi_ += other.psi_;
        return *this;
    }

    //  Comparison

    bool approx_equals(const Wave& other, double tol = DEFAULT_TOL) const {
        return approx_eq(psi_, other.psi_, tol);
    }

    //  Display 

    std::string to_string() const {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(6);
        ss << "psi = " << psi_.real();
        if (psi_.imag() >= 0) ss << " + " << psi_.imag() << "i";
        else ss << " - " << -psi_.imag() << "i";
        ss << "  (I = " << std::setprecision(4) << get_intensity()
           << ", phi = " << std::setprecision(1) << phase_degrees() << "deg)";
        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Wave& w) {
        return os << w.to_string();
    }

private:
    Complex psi_;  // The complex optical field amplitude
};


// ═════════════════════════════════════════════════════════════════════
//  WaveChannel — Multi-Channel Optical Signal
// ═════════════════════════════════════════════════════════════════════
// Represents N parallel optical channels through a shared medium.
// Analogous to N wires in a bus, but each carries a complex wave.
//
// In photonic computing, multiple channels can share the same
// physical medium via Wavelength-Division Multiplexing (WDM) —
// each channel at a different wavelength of light.
//
// Used by multi-channel operations like Softmax/XGM where channels
// must compete for shared optical gain.

class WaveChannel {
public:
    // Constructors 

    WaveChannel() = default;

    // N channels, all dark (vacuum)
    explicit WaveChannel(size_t n) : channels_(n) {}

    // From initializer list
    WaveChannel(std::initializer_list<Wave> waves)
        : channels_(waves) {}

    // From vector
    explicit WaveChannel(std::vector<Wave> waves)
        : channels_(std::move(waves)) {}

    // Accessors 

    size_t size() const { return channels_.size(); }
    bool empty() const { return channels_.empty(); }

    Wave& operator[](size_t i) { return channels_[i]; }
    const Wave& operator[](size_t i) const { return channels_[i]; }

    Wave& at(size_t i) { return channels_.at(i); }
    const Wave& at(size_t i) const { return channels_.at(i); }

    // Iterators

    auto begin() { return channels_.begin(); }
    auto end() { return channels_.end(); }
    auto begin() const { return channels_.begin(); }
    auto end() const { return channels_.end(); }

    // Aggregate Operations

    // Total optical power across all channels: Σ |ψ_j|²
    Intensity total_power() const {
        Intensity total = 0.0;
        for (const auto& w : channels_) {
            total += w.get_intensity();
        }
        return total;
    }

    // Intensity vector
    std::vector<Intensity> intensities() const {
        std::vector<Intensity> result;
        result.reserve(channels_.size());
        for (const auto& w : channels_) {
            result.push_back(w.get_intensity());
        }
        return result;
    }

    // Mutation

    void push_back(const Wave& w) { channels_.push_back(w); }
    void resize(size_t n) { channels_.resize(n); }

    // Display

    friend std::ostream& operator<<(std::ostream& os, const WaveChannel& wc) {
        os << "WaveChannel[" << wc.size() << "] {\n";
        for (size_t i = 0; i < wc.size(); ++i) {
            os << "  ch[" << i << "] " << wc[i] << "\n";
        }
        os << "  total_power = " << std::fixed << std::setprecision(4)
           << wc.total_power() << "\n}";
        return os;
    }

private:
    std::vector<Wave> channels_;
};

}  // namespace phx
