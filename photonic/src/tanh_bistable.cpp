// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Tanh → Optical Bistability Implementation
// ═══════════════════════════════════════════════════════════════════════
//
//  Physics:
//    Optical bistability occurs in non-linear cavities where the
//    circulating intensity affects the cavity's resonance condition.
//    A symmetrically driven dual-output MZI can be configured to
//    produce differential outputs that map to [-1, +1]:
//
//    - Positive output (+1): wave in-phase with reference
//    - Negative output (-1): wave π-shifted (out-of-phase)
//
//    The tanh function provides the smooth transition between
//    these two states. Physically, this arises from the competition
//    between constructive and destructive interference in the two
//    output ports of the MZI.
//
//    The additional phase factor e^(iΔθ) models the cavity's
//    round-trip phase accumulation or an external phase bias.
//
//  Wave Transformation:
//    ψ_out = ψ_in · tanh(β|ψ_in|²) · e^(iΔθ)
//
//  Properties:
//    - tanh(0) = 0: zero input → complete destructive interference
//    - tanh(∞) → 1: high input → full constructive interference
//    - Smooth, differentiable transition
//    - β controls steepness (higher β = sharper switching)
//
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/tanh_bistable.h"
#include <cmath>
#include <sstream>

namespace phx {

OpticalBistable::OpticalBistable(double beta, Phase delta_theta)
    : beta_(beta), delta_theta_(delta_theta) {}

Wave OpticalBistable::process(const Wave& input) const {
    Intensity I_in = input.get_intensity();

    // Compute tanh response
    // tanh(β·|ψ|²) maps intensity to [-1, +1]
    // At zero: tanh(0) = 0 → complete destructive interference
    // At high intensity: tanh → ±1 → full transmission
    double tanh_response = std::tanh(beta_ * I_in);

    // Apply phase factor e^(iΔθ)
    // This models the cavity's accumulated phase or external bias
    Complex phase_factor = std::polar(1.0, delta_theta_);

    // Combine: ψ_out = ψ_in · tanh(β|ψ_in|²) · e^(iΔθ)
    Complex output_amplitude = input.amplitude() * tanh_response * phase_factor;

    return Wave(output_amplitude);
}

std::string OpticalBistable::name() const {
    return "Optical Bistable (Tanh)";
}

std::string OpticalBistable::description() const {
    std::ostringstream ss;
    ss << "Non-linear cavity / dual-output MZI. "
       << "beta = " << beta_
       << ", delta_theta = " << to_degrees(delta_theta_) << " deg"
       << ". Smooth [-1,1] phase transition.";
    return ss.str();
}

// ── Factory ─────────────────────────────────────────────────────────
std::unique_ptr<PhotonicElement> create_tanh_bistable(const ParamMap& params) {
    double beta = 1.0;
    Phase delta_theta = 0.0;
    auto it = params.find("beta");
    if (it != params.end()) beta = it->second;
    it = params.find("delta_theta");
    if (it != params.end()) delta_theta = it->second;
    return std::make_unique<OpticalBistable>(beta, delta_theta);
}

}  // namespace phx
