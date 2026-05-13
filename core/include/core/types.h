#pragma once
// ═══════════════════════════════════════════════════════════════════════
//  PHX-PTC :: Core Type Definitions
//  Photonic Computing Platform — Foundation Types
// ═══════════════════════════════════════════════════════════════════════

#include <complex>
#include <cmath>
#include <limits>

namespace phx {

// ── Fundamental types ───────────────────────────────────────────────
// In photonic computing, data is carried by electromagnetic waves.
// A wave is fully described by a complex amplitude ψ = |ψ|·e^(iφ)
// where |ψ| is the field magnitude and φ is the optical phase.

using Complex   = std::complex<double>;   // Complex wave amplitude (ψ)
using Intensity = double;                  // Optical power: |ψ|²
using Phase     = double;                  // Phase angle in radians

// ── Physical constants ──────────────────────────────────────────────
constexpr double PI          = 3.14159265358979323846;
constexpr double TWO_PI      = 2.0 * PI;
constexpr double HALF_PI     = PI / 2.0;
constexpr double QUARTER_PI  = PI / 4.0;
constexpr double SQRT2       = 1.41421356237309504880;
constexpr double INV_SQRT2   = 1.0 / 1.41421356237309504880;

// Speed of light in vacuum (m/s)
constexpr double SPEED_OF_LIGHT = 299792458.0;

// Planck's constant (J·s)
constexpr double PLANCK = 6.62607015e-34;

// Reduced Planck's constant (J·s)
constexpr double HBAR = PLANCK / TWO_PI;

// ── Tolerance for floating-point comparison ─────────────────────────
constexpr double DEFAULT_TOL = 1e-9;

inline bool approx_eq(double a, double b, double tol = DEFAULT_TOL) {
    return std::abs(a - b) < tol;
}

inline bool approx_eq(Complex a, Complex b, double tol = DEFAULT_TOL) {
    return std::abs(a - b) < tol;
}

// ── Intensity from complex amplitude ────────────────────────────────
// |ψ|² — the measurable optical power
inline Intensity intensity(const Complex& psi) {
    return std::norm(psi);  // std::norm returns |z|² for complex
}

// ── Phase from complex amplitude ────────────────────────────────────
// arg(ψ) — the optical phase in radians [-π, π]
inline Phase phase(const Complex& psi) {
    return std::arg(psi);
}

// ── Degrees ↔ Radians ───────────────────────────────────────────────
inline double to_degrees(double radians) {
    return radians * 180.0 / PI;
}

inline double to_radians(double degrees) {
    return degrees * PI / 180.0;
}

}  // namespace phx
