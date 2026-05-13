// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Tanh / Optical Bistability Unit Tests
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/tanh_bistable.h"
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

TEST(zero_input_dark) {
    OpticalBistable ob(1.0);
    ASSERT_TRUE(ob.process(Wave()).is_dark());
    ASSERT_TRUE(ob.process(Wave::from_intensity(0.0)).is_dark());
}

TEST(tanh_response_formula) {
    OpticalBistable ob(1.0);
    double I_in = 2.0;
    Wave w_out = ob.process(Wave::from_intensity(I_in));
    double t = std::tanh(I_in);
    ASSERT_APPROX(w_out.get_intensity(), I_in * t * t);
}

TEST(saturation_high_intensity) {
    OpticalBistable ob(1.0);
    Wave w_in = Wave::from_intensity(100.0);
    Wave w_out = ob.process(w_in);
    ASSERT_TRUE(w_out.get_intensity() / w_in.get_intensity() > 0.999);
}

TEST(monotonic_output) {
    OpticalBistable ob(1.0);
    double prev = 0.0;
    for (int i = 1; i <= 50; ++i) {
        double I = i * 0.2;
        double Io = ob.process(Wave::from_intensity(I)).get_intensity();
        ASSERT_TRUE(Io >= prev - 1e-10);
        prev = Io;
    }
}

TEST(beta_steepness) {
    OpticalBistable slow(0.1), fast(10.0);
    double I = 1.0;
    ASSERT_TRUE(fast.process(Wave::from_intensity(I)).get_intensity()
              > slow.process(Wave::from_intensity(I)).get_intensity());
}

TEST(phase_offset) {
    OpticalBistable ob(1.0, PI / 2.0);
    Wave w_out = ob.process(Wave::from_intensity(2.0));
    ASSERT_APPROX(w_out.get_phase(), PI / 2.0);
}

TEST(no_phase_default) {
    OpticalBistable ob(1.0);
    Wave w_out = ob.process(Wave::from_intensity(2.0));
    ASSERT_APPROX(w_out.get_phase(), 0.0);
}

TEST(bounded_transmission) {
    OpticalBistable ob(1.0);
    for (int i = 0; i <= 100; ++i) {
        double I = i * 0.1;
        ASSERT_TRUE(ob.process(Wave::from_intensity(I)).get_intensity() <= I + 1e-10);
    }
}

TEST(complex_phase_preservation) {
    OpticalBistable ob(1.0);
    Wave w_in = Wave::from_polar(2.0, PI / 6.0);
    ASSERT_APPROX(ob.process(w_in).get_phase(), PI / 6.0);
}

TEST(factory_creation) {
    auto ob = create_tanh_bistable({{"beta", 2.0}, {"delta_theta", 1.0}});
    ASSERT_TRUE(!ob->process(Wave::from_intensity(1.0)).is_dark());
}

int main() {
    std::cout << "\n  === PHX-PTC :: Tanh/Bistable Tests ===\n\n";
    std::cout << "\n  Results: " << tests_passed << " passed, "
              << tests_failed << " failed\n\n";
    return tests_failed > 0 ? 1 : 0;
}
