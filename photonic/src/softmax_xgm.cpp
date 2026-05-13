// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Softmax → Cross-Gain Modulation (XGM) Implementation
// ═══════════════════════════════════════════════════════════════════════
//
//  Physics:
//    In a shared laser gain medium, multiple optical channels
//    compete for the same pool of excited atoms/carriers. The
//    channel with the highest input intensity stimulates emission
//    first, depleting the gain available to weaker channels.
//
//    This gain competition naturally implements softmax:
//    - The exponential arises from stimulated emission rate
//      (proportional to exp(gain · intensity))
//    - The normalization arises from the finite shared photon pool
//    - Higher γ = stronger competition = more winner-take-all
//
//    The transformation modifies only the magnitude of each channel
//    wave — the phase of each channel is preserved because XGM
//    operates on photon populations, not field interference.
//
//  Wave Transformation:
//    ψ_out,j = ψ_in,j · √( exp(γ|ψ_in,j|²) / Σ_k exp(γ|ψ_in,k|²) )
//
//  Numerical Stability:
//    We use the log-sum-exp trick to prevent overflow:
//    log(Σ exp(γI_k)) = max_k(γI_k) + log(Σ exp(γI_k - max))
//
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/softmax_xgm.h"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <stdexcept>

namespace phx {

CrossGainModulation::CrossGainModulation(double gamma)
    : gamma_(gamma) {}

WaveChannel CrossGainModulation::process(const WaveChannel& inputs) const {
    if (inputs.empty()) {
        return WaveChannel();
    }

    // Single channel: no competition → full gain (softmax of 1 element = 1.0)
    if (inputs.size() == 1) {
        return inputs;  // ψ_out = ψ_in (gain factor = √1 = 1)
    }

    const size_t N = inputs.size();

    // ── Step 1: Compute γ·I_j for each channel ─────────────────────
    std::vector<double> gamma_I(N);
    double max_val = -1e300;  // for log-sum-exp stability

    for (size_t j = 0; j < N; ++j) {
        gamma_I[j] = gamma_ * inputs[j].get_intensity();
        if (gamma_I[j] > max_val) max_val = gamma_I[j];
    }

    // ── Step 2: Log-Sum-Exp for numerical stability ─────────────────
    //   log(Σ exp(γI_k)) = max + log(Σ exp(γI_k - max))
    double sum_exp = 0.0;
    for (size_t k = 0; k < N; ++k) {
        sum_exp += std::exp(gamma_I[k] - max_val);
    }
    double log_sum_exp = max_val + std::log(sum_exp);

    // ── Step 3: Compute output waves ────────────────────────────────
    WaveChannel output(N);

    for (size_t j = 0; j < N; ++j) {
        // softmax_j = exp(γI_j) / Σ exp(γI_k)
        //           = exp(γI_j - log_sum_exp)
        double log_softmax_j = gamma_I[j] - log_sum_exp;
        double softmax_j = std::exp(log_softmax_j);

        // Gain factor: √(softmax_j)
        // Applied to field amplitude (not intensity)
        double gain_factor = std::sqrt(softmax_j);

        // Preserve input phase, scale magnitude by gain factor
        output[j] = Wave(inputs[j].amplitude() * gain_factor);
    }

    return output;
}

std::vector<double> CrossGainModulation::probabilities(
    const WaveChannel& inputs) const
{
    const size_t N = inputs.size();
    std::vector<double> probs(N);

    if (N == 0) return probs;
    if (N == 1) { probs[0] = 1.0; return probs; }

    // Same log-sum-exp computation
    std::vector<double> gamma_I(N);
    double max_val = -1e300;

    for (size_t j = 0; j < N; ++j) {
        gamma_I[j] = gamma_ * inputs[j].get_intensity();
        if (gamma_I[j] > max_val) max_val = gamma_I[j];
    }

    double sum_exp = 0.0;
    for (size_t k = 0; k < N; ++k) {
        sum_exp += std::exp(gamma_I[k] - max_val);
    }
    double log_sum_exp = max_val + std::log(sum_exp);

    for (size_t j = 0; j < N; ++j) {
        probs[j] = std::exp(gamma_I[j] - log_sum_exp);
    }

    return probs;
}

std::string CrossGainModulation::name() const {
    return "Cross-Gain Modulation (Softmax)";
}

std::string CrossGainModulation::description() const {
    std::ostringstream ss;
    ss << "Shared laser cavity with gain competition. "
       << "gamma = " << gamma_
       << ". Winner-take-all at high gamma, uniform at low gamma.";
    return ss.str();
}

// ── Factory ─────────────────────────────────────────────────────────
std::unique_ptr<MultiChannelElement> create_softmax_xgm(const ParamMap& params) {
    double gamma = 1.0;
    auto it = params.find("gamma");
    if (it != params.end()) gamma = it->second;
    return std::make_unique<CrossGainModulation>(gamma);
}

// ── General activation factory ──────────────────────────────────────
std::unique_ptr<PhotonicElement> create_activation(
    const std::string& name, const ParamMap& params)
{
    if (name == "relu" || name == "sa" || name == "saturable_absorber")
        return create_relu_sa(params);
    if (name == "sigmoid" || name == "mzi" || name == "mach_zehnder")
        return create_sigmoid_mzi(params);
    if (name == "tanh" || name == "bistable" || name == "optical_bistable")
        return create_tanh_bistable(params);

    throw std::invalid_argument("Unknown activation: " + name
        + ". Valid: relu, sigmoid, tanh (softmax requires MultiChannelElement)");
}

}  // namespace phx
