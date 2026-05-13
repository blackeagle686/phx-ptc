#pragma once
// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: ReLU → Saturable Absorber / Optical Thresholding
// ═══════════════════════════════════════════════════════════════════════
//
//  Physical Device: Saturable Absorber (SA) or Optical Discriminator
//
//  Behavior:
//    At low light intensities, the material is highly opaque and
//    absorbs all photons (output is zero). Once the input intensity
//    crosses a critical threshold (I_threshold), the material
//    bleaches — becomes completely transparent — letting the wave
//    pass through with reduced amplitude.
//
//  Wave Transformation:
//    ψ_out = 0                                       if |ψ_in|² ≤ I_threshold
//    ψ_out = ψ_in · √(1 - I_threshold / |ψ_in|²)   if |ψ_in|² > I_threshold
//
//  Properties:
//    - Hard cutoff below threshold (complete absorption)
//    - Phase-preserving above threshold
//    - Continuous at boundary (output → 0 as I → I_threshold from above)
//    - Energy conservation: I_out ≤ I_in always
//
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/element.h"

namespace phx {

class SaturableAbsorber : public PhotonicElement {
public:
    // Construct with saturation intensity threshold
    // Default: I_threshold = 0.5 (moderate bleaching point)
    explicit SaturableAbsorber(Intensity threshold = 0.5);

    Wave process(const Wave& input) const override;
    std::string name() const override;
    std::string description() const override;

    // ── Accessors ───────────────────────────────────────────────────
    Intensity threshold() const { return threshold_; }
    void set_threshold(Intensity t) { threshold_ = t; }

private:
    Intensity threshold_;
};

}  // namespace phx
