#pragma once
// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Tanh → Optical Bistability / Phase-Shifted MZI
// ═══════════════════════════════════════════════════════════════════════
//
//  Physical Device: Non-linear Cavity or symmetrically driven
//                   Dual-Output MZI
//
//  Behavior:
//    Tanh maps inputs to a [-1, 1] range. In photonics, a negative
//    value signifies a 180° (π radians) phase shift. By splitting
//    the wave and utilizing differential destructive interference,
//    the output wave transitions smoothly between being completely
//    in-phase (+1) and completely out-of-phase (-1).
//
//  Wave Transformation:
//    ψ_out = ψ_in · tanh(β|ψ_in|²) · e^(iΔθ)
//
//  Parameters:
//    β  — non-linear response steepness
//    Δθ — additional phase offset from bistable cavity
//
//  Properties:
//    - Maps to [-1, 1] range via tanh
//    - Negative values encode π phase shifts
//    - Smooth transition between in-phase and out-of-phase
//    - At zero input: tanh(0) = 0 → complete destructive interference
//    - Saturates to ±1 at high intensity
//
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/element.h"

namespace phx {

class OpticalBistable : public PhotonicElement {
public:
    // Construct with response steepness and phase offset
    // Default: β = 1.0, Δθ = 0.0
    explicit OpticalBistable(double beta = 1.0, Phase delta_theta = 0.0);

    Wave process(const Wave& input) const override;
    std::string name() const override;
    std::string description() const override;

    // ── Accessors ───────────────────────────────────────────────────
    double beta() const { return beta_; }
    void set_beta(double b) { beta_ = b; }

    Phase delta_theta() const { return delta_theta_; }
    void set_delta_theta(Phase dt) { delta_theta_ = dt; }

private:
    double beta_;
    Phase  delta_theta_;
};

}  // namespace phx
