// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Softmax / Cross-Gain Modulation Unit Tests
// ═══════════════════════════════════════════════════════════════════════

#include "photonic/softmax_xgm.h"
#include "core/wave.h"
#include "core/types.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <numeric>

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
#define ASSERT_APPROX(a, b) do { if (std::abs((a)-(b)) > 1e-6) throw std::runtime_error(#a " != " #b); } while(0)

TEST(empty_channel) {
    CrossGainModulation xgm(1.0);
    WaveChannel empty;
    WaveChannel out = xgm.process(empty);
    ASSERT_TRUE(out.empty());
}

TEST(single_channel_identity) {
    CrossGainModulation xgm(1.0);
    WaveChannel input = {Wave::from_intensity(2.0)};
    WaveChannel out = xgm.process(input);
    ASSERT_TRUE(out.size() == 1);
    // softmax of single element = 1.0 → gain = √1 = 1 → unchanged
    ASSERT_APPROX(out[0].get_intensity(), input[0].get_intensity());
}

TEST(probabilities_sum_to_one) {
    CrossGainModulation xgm(1.0);
    WaveChannel input = {
        Wave::from_intensity(1.0),
        Wave::from_intensity(2.0),
        Wave::from_intensity(0.5),
        Wave::from_intensity(3.0)
    };
    auto probs = xgm.probabilities(input);
    double sum = 0;
    for (auto p : probs) sum += p;
    ASSERT_APPROX(sum, 1.0);
}

TEST(uniform_input_uniform_output) {
    CrossGainModulation xgm(1.0);
    WaveChannel input = {
        Wave::from_intensity(1.0),
        Wave::from_intensity(1.0),
        Wave::from_intensity(1.0),
        Wave::from_intensity(1.0)
    };
    auto probs = xgm.probabilities(input);
    for (auto p : probs) {
        ASSERT_APPROX(p, 0.25);  // 1/4 each
    }
}

TEST(winner_take_all_high_gamma) {
    CrossGainModulation xgm(100.0);  // Very high gamma
    WaveChannel input = {
        Wave::from_intensity(1.0),
        Wave::from_intensity(2.0),
        Wave::from_intensity(3.0)  // Winner
    };
    auto probs = xgm.probabilities(input);
    // With very high gamma, channel 2 (I=3.0) should dominate
    ASSERT_TRUE(probs[2] > 0.99);
    ASSERT_TRUE(probs[0] < 0.01);
    ASSERT_TRUE(probs[1] < 0.01);
}

TEST(phase_preservation) {
    CrossGainModulation xgm(1.0);
    WaveChannel input = {
        Wave::from_polar(1.0, PI / 3.0),
        Wave::from_polar(2.0, PI / 6.0),
        Wave::from_polar(1.5, -PI / 4.0)
    };
    WaveChannel out = xgm.process(input);
    for (size_t j = 0; j < input.size(); ++j) {
        ASSERT_APPROX(out[j].get_phase(), input[j].get_phase());
    }
}

TEST(stronger_channel_gets_more_gain) {
    CrossGainModulation xgm(1.0);
    WaveChannel input = {
        Wave::from_intensity(1.0),
        Wave::from_intensity(3.0)
    };
    auto probs = xgm.probabilities(input);
    ASSERT_TRUE(probs[1] > probs[0]);
}

TEST(output_intensity_formula) {
    CrossGainModulation xgm(1.0);
    WaveChannel input = {
        Wave::from_intensity(1.0),
        Wave::from_intensity(2.0)
    };
    WaveChannel out = xgm.process(input);
    auto probs = xgm.probabilities(input);

    // I_out,j = I_in,j · softmax_j
    for (size_t j = 0; j < input.size(); ++j) {
        double expected = input[j].get_intensity() * probs[j];
        ASSERT_APPROX(out[j].get_intensity(), expected);
    }
}

TEST(low_gamma_near_uniform) {
    CrossGainModulation xgm(0.001);  // Very low gamma
    WaveChannel input = {
        Wave::from_intensity(1.0),
        Wave::from_intensity(5.0),
        Wave::from_intensity(10.0)
    };
    auto probs = xgm.probabilities(input);
    // With very low gamma, all channels get near-equal probability
    for (auto p : probs) {
        ASSERT_TRUE(std::abs(p - 1.0/3.0) < 0.05);
    }
}

TEST(factory_creation) {
    auto xgm = create_softmax_xgm({{"gamma", 2.0}});
    WaveChannel input = {Wave::from_intensity(1.0), Wave::from_intensity(2.0)};
    WaveChannel out = xgm->process(input);
    ASSERT_TRUE(out.size() == 2);
}

int main() {
    std::cout << "\n  === PHX-PTC :: Softmax/XGM Tests ===\n\n";
    std::cout << "\n  Results: " << tests_passed << " passed, "
              << tests_failed << " failed\n\n";
    return tests_failed > 0 ? 1 : 0;
}
