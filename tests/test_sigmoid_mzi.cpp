// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Sigmoid / MZI Modulation Unit Tests
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/sigmoid_mzi.h"
#include "core/wave.h"
#include "core/types.h"
#include <iostream>
#include <cassert>
#include <cmath>

using namespace phx;

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    static void test_##name(); \
    struct Register_##name { Register_##name() { \
        std::cout << "  [TEST] " #name "... "; \
        try { test_##name(); tests_passed++; std::cout << "PASS\n"; } \
        catch (...) { tests_failed++; std::cout << "FAIL\n"; } \
    }} reg_##name; \
    void test_##name()

#define ASSERT_TRUE(cond) do { if (!(cond)) throw std::runtime_error(#cond); } while(0)
#define ASSERT_APPROX(a, b) do { if (std::abs((a)-(b)) > 1e-8) throw std::runtime_error(#a " != " #b); } while(0)

// ─────────────────────────────────────────────────────────────────────

TEST(zero_input_quadrature_bias) {
    MachZehnderSigmoid mzi(1.0);

    // At zero input: ψ_out = ψ_in · cos(α·0 + π/4) = ψ_in · cos(π/4)
    // cos(π/4) = 1/√2 ≈ 0.7071
    // But ψ_in = 0, so ψ_out = 0
    Wave w = mzi.process(Wave());
    ASSERT_TRUE(w.is_dark());
}

TEST(quadrature_transmission) {
    MachZehnderSigmoid mzi(1.0);

    // I_in = 0: cos(π/4) = 1/√2
    // Field transmission at I=0 should be cos(π/4)
    Wave w_in(1.0);  // amplitude = 1
    Wave w_out = mzi.process(w_in);

    // ψ_out = 1.0 · cos(1.0·1.0 + π/4) = cos(1.0 + 0.7854)
    double expected_cos = std::cos(1.0 * 1.0 + QUARTER_PI);
    ASSERT_APPROX(w_out.amplitude().real(), expected_cos);
}

TEST(bounded_output_intensity) {
    MachZehnderSigmoid mzi(1.0);

    // cos² is always in [0, 1], so I_out ≤ I_in
    for (int i = 0; i <= 100; ++i) {
        double I_in = i * 0.1;
        Wave w_out = mzi.process(Wave::from_intensity(I_in));
        ASSERT_TRUE(w_out.get_intensity() <= I_in + 1e-10);
    }
}

TEST(cos_squared_transfer) {
    MachZehnderSigmoid mzi(1.0);

    // Verify I_out = I_in · cos²(α·I_in + π/4)
    double I_in = 2.0;
    Wave w_out = mzi.process(Wave::from_intensity(I_in));

    double cos_val = std::cos(1.0 * I_in + QUARTER_PI);
    double expected_I_out = I_in * cos_val * cos_val;
    ASSERT_APPROX(w_out.get_intensity(), expected_I_out);
}

TEST(s_shaped_response) {
    MachZehnderSigmoid mzi(0.5);

    // Intensity transfer should show S-shaped behavior
    // At low I: cos(π/4) ≈ 0.707 transmission
    // As I increases: cos decreases (initially)
    Wave w_low = mzi.process(Wave::from_intensity(0.01));
    Wave w_mid = mzi.process(Wave::from_intensity(1.0));

    // At low input, transmission is near cos²(π/4) = 0.5
    double T_low = w_low.get_intensity() / 0.01;
    ASSERT_APPROX(T_low, std::cos(0.5 * 0.01 + QUARTER_PI) * std::cos(0.5 * 0.01 + QUARTER_PI));
}

TEST(phase_flip_negative_cosine) {
    MachZehnderSigmoid mzi(1.0);

    // When cos(α·I + π/4) goes negative, the output field gets a π phase shift
    // cos(x) < 0 when x > π/2
    // α·I + π/4 > π/2 → I > π/4 ≈ 0.785
    Wave w_in = Wave::from_intensity(2.0);  // I = 2.0 > π/4
    Wave w_out = mzi.process(w_in);

    double cos_val = std::cos(1.0 * 2.0 + QUARTER_PI);
    if (cos_val < 0) {
        // Output phase should be π (phase flip)
        ASSERT_APPROX(std::abs(w_out.get_phase()), PI);
    }
}

TEST(alpha_parameter_effect) {
    // Higher alpha → faster oscillation of the transfer function
    MachZehnderSigmoid mzi_low(0.1);
    MachZehnderSigmoid mzi_high(10.0);

    double I = 1.0;
    Wave out_low = mzi_low.process(Wave::from_intensity(I));
    Wave out_high = mzi_high.process(Wave::from_intensity(I));

    // Different alpha should give different outputs at same input
    // (unless they happen to coincide, which is unlikely)
    double diff = std::abs(out_low.get_intensity() - out_high.get_intensity());
    ASSERT_TRUE(diff > 1e-6);
}

TEST(periodicity) {
    MachZehnderSigmoid mzi(1.0);

    // cos is periodic: cos(x) = cos(x + 2π)
    // So α·I₁ + π/4 and α·I₂ + π/4 differ by 2π → same output transmission
    double I1 = 0.5;
    double I2 = I1 + TWO_PI;  // shifted by one full period in phase

    Wave w1 = mzi.process(Wave::from_intensity(I1));
    Wave w2 = mzi.process(Wave::from_intensity(I2));

    // Transmission (cos²) should be the same
    double T1 = w1.get_intensity() / I1;
    double T2 = w2.get_intensity() / I2;
    ASSERT_APPROX(T1, T2);
}

TEST(factory_creation) {
    auto mzi = create_sigmoid_mzi({{"alpha", 2.0}});
    Wave w = mzi->process(Wave::from_intensity(1.0));
    double expected = std::cos(2.0 * 1.0 + QUARTER_PI);
    ASSERT_APPROX(w.amplitude().real(), std::sqrt(1.0) * expected);
}

// ─────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "\n  ═══ PHX-PTC :: Sigmoid/MZI Tests ═══\n\n";
    std::cout << "\n  Results: " << tests_passed << " passed, "
              << tests_failed << " failed\n\n";
    return tests_failed > 0 ? 1 : 0;
}
