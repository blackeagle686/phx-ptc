#pragma once
// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Softmax → Cross-Gain Modulation (XGM)
// ═══════════════════════════════════════════════════════════════════════
//
//  Physical Device: Laser Diode Array with shared optical gain
//                   or Cross-Gain Modulation (XGM)
//
//  Behavior:
//    Softmax requires global normalization across a vector of
//    channels (Σ exp(x_i)). In photonics, this is achieved by
//    routing all wave channels into a shared optical gain medium.
//    The channel with the highest initial intensity rapidly
//    consumes the shared photon pool (gain competition),
//    exponentially suppressing the weaker channels.
//
//  Wave Transformation:
//    ψ_out,j = ψ_in,j · √( exp(γ|ψ_in,j|²) / Σ_k exp(γ|ψ_in,k|²) )
//
//  Parameters:
//    γ — gain competition coefficient
//
//  Properties:
//    - Operates on WaveChannel (multi-channel, not single-wave)
//    - Output intensity ratios form valid probability distribution
//    - High γ → winner-take-all (hard selection)
//    - Low γ  → near-uniform distribution
//    - Preserves input phase per channel (only magnitudes compete)
//
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/element.h"

namespace phx {

class CrossGainModulation : public MultiChannelElement {
public:
    // Construct with gain competition coefficient
    // Default: γ = 1.0
    explicit CrossGainModulation(double gamma = 1.0);

    WaveChannel process(const WaveChannel& inputs) const override;
    std::string name() const override;
    std::string description() const override;

    // ── Accessors ───────────────────────────────────────────────────
    double gamma() const { return gamma_; }
    void set_gamma(double g) { gamma_ = g; }

    // ── Convenience ─────────────────────────────────────────────────
    // Compute softmax probabilities without constructing output waves
    std::vector<double> probabilities(const WaveChannel& inputs) const;

private:
    double gamma_;
};

}  // namespace phx
