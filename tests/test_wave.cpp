// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Wave Unit Tests
// ═══════════════════════════════════════════════════════════════════════

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

TEST(default_wave_is_dark) {
    Wave w;
    ASSERT_TRUE(w.is_dark());
    ASSERT_APPROX(w.get_intensity(), 0.0);
    ASSERT_APPROX(w.magnitude(), 0.0);
}

TEST(real_amplitude_construction) {
    Wave w(2.0);
    ASSERT_APPROX(w.magnitude(), 2.0);
    ASSERT_APPROX(w.get_intensity(), 4.0);  // |2|² = 4
    ASSERT_APPROX(w.get_phase(), 0.0);      // real → phase 0
}

TEST(complex_amplitude_construction) {
    Wave w(Complex(1.0, 1.0));
    ASSERT_APPROX(w.get_intensity(), 2.0);  // |1+i|² = 2
    ASSERT_APPROX(w.magnitude(), std::sqrt(2.0));
    ASSERT_APPROX(w.get_phase(), PI / 4.0); // arg(1+i) = π/4
}

TEST(from_polar) {
    Wave w = Wave::from_polar(3.0, PI / 2.0);
    ASSERT_APPROX(w.magnitude(), 3.0);
    ASSERT_APPROX(w.get_intensity(), 9.0);
    ASSERT_APPROX(w.get_phase(), PI / 2.0);
}

TEST(from_intensity) {
    Wave w = Wave::from_intensity(4.0);
    ASSERT_APPROX(w.get_intensity(), 4.0);
    ASSERT_APPROX(w.magnitude(), 2.0);
    ASSERT_APPROX(w.get_phase(), 0.0);  // default phase = 0
}

TEST(from_intensity_with_phase) {
    Wave w = Wave::from_intensity(1.0, PI);
    ASSERT_APPROX(w.get_intensity(), 1.0);
    ASSERT_APPROX(std::abs(w.get_phase()), PI);  // phase = π
}

TEST(attenuation) {
    Wave w(2.0);
    Wave att = w.attenuate(0.5);
    ASSERT_APPROX(att.magnitude(), 1.0);
    ASSERT_APPROX(att.get_intensity(), 1.0);
}

TEST(phase_shift) {
    Wave w(1.0);  // real, phase = 0
    Wave shifted = w.phase_shift(PI / 2.0);
    ASSERT_APPROX(shifted.magnitude(), 1.0);          // magnitude unchanged
    ASSERT_APPROX(shifted.get_phase(), PI / 2.0);     // phase shifted
    ASSERT_APPROX(shifted.get_intensity(), 1.0);       // intensity unchanged
}

TEST(superposition_constructive) {
    Wave a(1.0);
    Wave b(1.0);
    Wave sum = a + b;
    ASSERT_APPROX(sum.magnitude(), 2.0);
    ASSERT_APPROX(sum.get_intensity(), 4.0);
}

TEST(superposition_destructive) {
    Wave a(1.0);
    Wave b(-1.0);
    Wave sum = a + b;
    ASSERT_TRUE(sum.is_dark());
}

TEST(wave_channel_construction) {
    WaveChannel wc = {Wave(1.0), Wave(2.0), Wave(3.0)};
    ASSERT_TRUE(wc.size() == 3);
    ASSERT_APPROX(wc[0].magnitude(), 1.0);
    ASSERT_APPROX(wc[1].magnitude(), 2.0);
    ASSERT_APPROX(wc[2].magnitude(), 3.0);
}

TEST(wave_channel_total_power) {
    WaveChannel wc = {
        Wave::from_intensity(1.0),
        Wave::from_intensity(2.0),
        Wave::from_intensity(3.0)
    };
    ASSERT_APPROX(wc.total_power(), 6.0);  // 1 + 2 + 3
}

TEST(wave_channel_intensities) {
    WaveChannel wc = {
        Wave::from_intensity(1.5),
        Wave::from_intensity(2.5)
    };
    auto I = wc.intensities();
    ASSERT_TRUE(I.size() == 2);
    ASSERT_APPROX(I[0], 1.5);
    ASSERT_APPROX(I[1], 2.5);
}

TEST(phase_degrees) {
    Wave w = Wave::from_polar(1.0, PI / 2.0);
    ASSERT_APPROX(w.phase_degrees(), 90.0);
}

// ─────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "\n  ═══ PHX-PTC :: Wave Tests ═══\n\n";
    // Tests auto-run via static registration
    std::cout << "\n  Results: " << tests_passed << " passed, "
              << tests_failed << " failed\n\n";
    return tests_failed > 0 ? 1 : 0;
}
