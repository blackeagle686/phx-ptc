// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: FFN Unit Tests
// ═══════════════════════════════════════════════════════════════════════

#include "ONN/include/ffn.h"
#include "core/wave.h"
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
        catch (const std::exception& e) { tests_failed++; std::cout << "FAIL (" << e.what() << ")\n"; } \
        catch (...) { tests_failed++; std::cout << "FAIL (Unknown Error)\n"; } \
    }} reg_##name; \
    void test_##name()

#define ASSERT_TRUE(cond) do { if (!(cond)) throw std::runtime_error(#cond); } while(0)
#define ASSERT_APPROX(a, b) do { if (std::abs((a)-(b)) > 1e-6) throw std::runtime_error(#a " != " #b); } while(0)

// ─────────────────────────────────────────────────────────────────────

TEST(ffn_default_initialization) {
    FFN network;
    WaveChannel input = {Wave(1.0), Wave(1.0)};
    network.set_input(input);
    network.set_activation("mzi");
    
    // Forward should init weights to 1.0 and bias to 0.0
    network.forward();
    
    const auto& output = network.get_output();
    ASSERT_TRUE(output.size() == 2);
    // Since weight is 1.0 and bias 0.0, output of MZI(1.0) should be Sigmoid(1.0)
    // We just verify it's not dark
    ASSERT_TRUE(output.total_power() > 0.0);
}

TEST(ffn_identity_transform) {
    FFN network;
    WaveChannel input = {Wave::from_intensity(1.0)};
    network.set_input(input);
    
    // Set identity weight and zero bias
    network.set_weights({Complex(1.0, 0.0)});
    network.set_bias({Complex(0.0, 0.0)});
    network.set_activation("saturable_absorber"); // ReLU-like
    
    network.forward();
    
    const auto& output = network.get_output();
    // Saturable Absorber with default threshold 0.5:
    // I_in = 1.0 -> I_out = 1.0 - 0.5 = 0.5
    ASSERT_APPROX(output[0].get_intensity(), 0.5);
}

TEST(ffn_bias_only) {
    FFN network;
    WaveChannel input = {Wave(0.0)}; // Dark input
    network.set_input(input);
    
    network.set_weights({Complex(1.0, 0.0)});
    // Add a bias wave with intensity 1.0
    network.set_bias({Complex(1.0, 0.0)});
    network.set_activation("saturable_absorber");
    
    network.forward();
    
    const auto& output = network.get_output();
    // I_in = 0, bias = 1 -> I_total = 1.0
    // I_out = 1.0 - 0.5 = 0.5
    ASSERT_APPROX(output[0].get_intensity(), 0.5);
}

TEST(ffn_multi_channel_softmax) {
    FFN network;
    WaveChannel input = {Wave::from_intensity(1.0), Wave::from_intensity(2.0)};
    network.set_input(input);
    network.set_activation("softmax");
    
    network.forward();
    
    const auto& output = network.get_output();
    ASSERT_TRUE(output.size() == 2);
    // Softmax should preserve total power or normalize it.
    // In our XGM model, it normalizes to a probability distribution.
    ASSERT_APPROX(output[0].get_intensity() + output[1].get_intensity(), 1.0);
}

// ─────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "\n  ═══ PHX-PTC :: FFN Tests ═══\n\n";
    std::cout << "\n  Results: " << tests_passed << " passed, "
              << tests_failed << " failed\n\n";
    return tests_failed > 0 ? 1 : 0;
}
