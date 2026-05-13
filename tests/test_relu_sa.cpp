// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: ReLU / Saturable Absorber Unit Tests
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/relu_sa.h"
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

TEST(zero_below_threshold) {
    SaturableAbsorber sa(1.0);  // threshold = 1.0

    // Input at 0.5 intensity → below threshold → output = 0
    Wave w_in = Wave::from_intensity(0.5);
    Wave w_out = sa.process(w_in);
    ASSERT_TRUE(w_out.is_dark());
}

TEST(zero_at_threshold) {
    SaturableAbsorber sa(1.0);

    // Input exactly at threshold → output = 0
    Wave w_in = Wave::from_intensity(1.0);
    Wave w_out = sa.process(w_in);
    ASSERT_TRUE(w_out.is_dark());
}

TEST(dark_input) {
    SaturableAbsorber sa(0.5);

    // No light → no output
    Wave w_in;
    Wave w_out = sa.process(w_in);
    ASSERT_TRUE(w_out.is_dark());
}

TEST(transparent_above_threshold) {
    SaturableAbsorber sa(1.0);

    // Input at 2.0 intensity → above threshold → some output
    Wave w_in = Wave::from_intensity(2.0);
    Wave w_out = sa.process(w_in);
    ASSERT_TRUE(w_out.get_intensity() > 0.0);
}

TEST(energy_conservation) {
    SaturableAbsorber sa(0.5);

    // Output intensity should always be ≤ input intensity
    for (int i = 0; i <= 100; ++i) {
        double I_in = i * 0.1;
        Wave w_out = sa.process(Wave::from_intensity(I_in));
        ASSERT_TRUE(w_out.get_intensity() <= I_in + 1e-10);
    }
}

TEST(output_intensity_formula) {
    // Above threshold: I_out = I_in - I_threshold
    SaturableAbsorber sa(1.0);

    Wave w_in = Wave::from_intensity(3.0);
    Wave w_out = sa.process(w_in);

    // I_out should be I_in - I_threshold = 3.0 - 1.0 = 2.0
    ASSERT_APPROX(w_out.get_intensity(), 2.0);
}

TEST(phase_preservation) {
    SaturableAbsorber sa(0.5);

    // Input with non-trivial phase
    Wave w_in = Wave::from_polar(2.0, PI / 3.0);  // I = 4.0, well above threshold
    Wave w_out = sa.process(w_in);

    // Phase should be preserved (SA doesn't rotate phase)
    ASSERT_APPROX(w_out.get_phase(), PI / 3.0);
}

TEST(boundary_continuity) {
    SaturableAbsorber sa(1.0);

    // Just above threshold: output should be near zero (continuous)
    Wave w_in = Wave::from_intensity(1.0001);
    Wave w_out = sa.process(w_in);

    // Output should be very small (near-continuous boundary)
    ASSERT_TRUE(w_out.get_intensity() < 0.01);
}

TEST(high_intensity_approaches_linear) {
    SaturableAbsorber sa(1.0);

    // At very high intensity, I_out ≈ I_in (threshold is negligible)
    Wave w_in = Wave::from_intensity(1000.0);
    Wave w_out = sa.process(w_in);

    double ratio = w_out.get_intensity() / w_in.get_intensity();
    ASSERT_TRUE(ratio > 0.999);  // Nearly full transmission
}

TEST(configurable_threshold) {
    SaturableAbsorber sa(2.0);
    ASSERT_APPROX(sa.threshold(), 2.0);

    // Below new threshold
    Wave w = sa.process(Wave::from_intensity(1.5));
    ASSERT_TRUE(w.is_dark());

    // Above new threshold
    w = sa.process(Wave::from_intensity(3.0));
    ASSERT_TRUE(!w.is_dark());
}

TEST(factory_creation) {
    auto sa = create_relu_sa({{"threshold", 0.75}});
    Wave w = sa->process(Wave::from_intensity(0.5));
    ASSERT_TRUE(w.is_dark());  // Below threshold

    w = sa->process(Wave::from_intensity(2.0));
    ASSERT_TRUE(!w.is_dark());  // Above threshold
}

// ─────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "\n  ═══ PHX-PTC :: ReLU/SA Tests ═══\n\n";
    std::cout << "\n  Results: " << tests_passed << " passed, "
              << tests_failed << " failed\n\n";
    return tests_failed > 0 ? 1 : 0;
}
