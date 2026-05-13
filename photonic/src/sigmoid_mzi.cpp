// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Sigmoid → Mach-Zehnder Interferometer Implementation
// ═══════════════════════════════════════════════════════════════════════
//
//  Physics:
//    A Mach-Zehnder Interferometer splits an input wave into two
//    arms, applies a relative phase shift between them, then
//    recombines them. The output intensity follows:
//
//      I_out = I_in · cos²(Δφ / 2)
//
//    where Δφ is the differential phase between the two arms.
//
//    When the phase shift is made intensity-dependent (via the
//    Kerr effect or electro-optic modulation):
//
//      Δφ = α|ψ_in|² + bias
//
//    Biasing at quadrature (π/4) places the operating point at
//    the steepest part of the cosine curve, producing a smooth,
//    S-shaped transfer function — the optical sigmoid.
//
//    The field transformation uses cos (not cos²) because the
//    wave amplitude is the square root of intensity:
//
//      ψ_out = ψ_in · cos(α|ψ_in|² + π/4)
//
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/sigmoid_mzi.h"
#include <cmath>
#include <sstream>

namespace phx {

MachZehnderSigmoid::MachZehnderSigmoid(double alpha)
    : alpha_(alpha) {}

Wave MachZehnderSigmoid::process(const Wave& input) const {
    Intensity I_in = input.get_intensity();

    // Compute the intensity-dependent phase in the MZI
    // Δφ = α·|ψ_in|² + π/4 (biased at quadrature)
    double phase_arg = alpha_ * I_in + QUARTER_PI;

    // Field transmission: cos(Δφ)
    // Note: this can go negative, which physically means the output
    // field has flipped phase by π — a real phenomenon in MZI devices
    double transmission = std::cos(phase_arg);

    // Apply to input wave amplitude
    return Wave(input.amplitude() * transmission);
}

std::string MachZehnderSigmoid::name() const {
    return "Mach-Zehnder Sigmoid";
}

std::string MachZehnderSigmoid::description() const {
    std::ostringstream ss;
    ss << "MZI modulation with quadrature bias. "
       << "alpha = " << alpha_
       << ". S-shaped cos^2 intensity response.";
    return ss.str();
}

// ── Factory ─────────────────────────────────────────────────────────
std::unique_ptr<PhotonicElement> create_sigmoid_mzi(const ParamMap& params) {
    double alpha = 1.0;
    auto it = params.find("alpha");
    if (it != params.end()) alpha = it->second;
    return std::make_unique<MachZehnderSigmoid>(alpha);
}

}  // namespace phx
