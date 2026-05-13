#pragma once
// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Sigmoid → Mach-Zehnder Interferometer Modulation
// ═══════════════════════════════════════════════════════════════════════
//
//  Physical Device: Electro-Optic Modulator (EOM) or biased MZI
//
//  Behavior:
//    An interference pattern natively produces a cosine-squared
//    intensity response (I_out ∝ cos²(θ)). By biasing the phase
//    shifter to its quadrature point (π/4), the transmission curve
//    perfectly mimics the smooth, bounded, S-shaped Sigmoid function.
//
//  Wave Transformation:
//    ψ_out = ψ_in · cos(α|ψ_in|² + π/4)
//
//  Parameters:
//    α — non-linear phase-shift coefficient (Kerr / electro-optic)
//
//  Properties:
//    - Bounded output (due to cosine envelope)
//    - S-shaped intensity response (cos² transfer function)
//    - Biased at quadrature → partial transmission at zero input
//    - Can produce phase flip (π shift) in negative cosine region
//
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/element.h"

namespace phx {

class MachZehnderSigmoid : public PhotonicElement {
public:
    // Construct with non-linear phase-shift coefficient
    // Default: α = 1.0
    explicit MachZehnderSigmoid(double alpha = 1.0);

    Wave process(const Wave& input) const override;
    std::string name() const override;
    std::string description() const override;

    // ── Accessors ───────────────────────────────────────────────────
    double alpha() const { return alpha_; }
    void set_alpha(double a) { alpha_ = a; }

private:
    double alpha_;
};

}  // namespace phx
