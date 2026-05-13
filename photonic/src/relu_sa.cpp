// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: ReLU → Saturable Absorber Implementation
// ═══════════════════════════════════════════════════════════════════════
//
//  Physics:
//    A saturable absorber is a material whose absorption coefficient
//    decreases as the incident light intensity increases. Below the
//    saturation threshold, the material is opaque (all photons are
//    absorbed by electronic transitions). Above the threshold, the
//    excited-state population is fully inverted and the material
//    becomes transparent (bleached).
//
//    The transmission factor √(1 - I_th/I_in) ensures:
//    1. Continuity at the boundary (→ 0 as I → I_th⁺)
//    2. Approaches unity for I_in >> I_th (full transparency)
//    3. Energy conservation: I_out = I_in - I_th (for I > I_th)
//
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/relu_sa.h"
#include <cmath>
#include <sstream>

namespace phx {

SaturableAbsorber::SaturableAbsorber(Intensity threshold)
    : threshold_(threshold) {}

Wave SaturableAbsorber::process(const Wave& input) const {
    Intensity I_in = input.get_intensity();

    // Below threshold: material is completely opaque
    // All photons absorbed → output is vacuum (dark)
    if (I_in <= threshold_) {
        return Wave();  // ψ_out = 0
    }

    // Above threshold: material bleaches
    // Transmission factor: √(1 - I_threshold / I_in)
    //
    // This gives: I_out = |ψ_in|² · (1 - I_th/|ψ_in|²) = |ψ_in|² - I_th
    // So the absorbed power is exactly I_threshold (constant loss above bleaching)
    double transmission = std::sqrt(1.0 - threshold_ / I_in);

    // Multiply input amplitude by transmission factor
    // Phase is preserved — SA doesn't rotate the optical phase
    return Wave(input.amplitude() * transmission);
}

std::string SaturableAbsorber::name() const {
    return "Saturable Absorber (ReLU)";
}

std::string SaturableAbsorber::description() const {
    std::ostringstream ss;
    ss << "Optical thresholding via saturable absorption. "
       << "I_threshold = " << threshold_
       << ". Opaque below threshold, transparent above.";
    return ss.str();
}

// ── Factory ─────────────────────────────────────────────────────────
std::unique_ptr<PhotonicElement> create_relu_sa(const ParamMap& params) {
    double threshold = 0.5;
    auto it = params.find("threshold");
    if (it != params.end()) threshold = it->second;
    return std::make_unique<SaturableAbsorber>(threshold);
}

}  // namespace phx
