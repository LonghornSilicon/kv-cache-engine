// test_sign_register.cpp — the RHT sign vector as a boot-loaded config register.
//
// Models the design decision agreed for the value write path: the per-channel
// +/-1 sign vector in front of the WHT (KVCacheEngine::sign_flips_) is a runtime
// config, not a tape-out constant. In silicon this is a sign register in front of
// the butterfly, loaded at boot; the multiply and butterfly are committed either
// way, only the sign source moves. Validated fixed==randomized at n=4000 on
// Qwen2-1.5B, so all +1 (plain fixed Hadamard) is shipped as the default.
//
// Asserts:
//   1. empty info.sign_flips -> seed-derived pattern (LEGACY: golden vectors
//      unchanged; -1s present, so it is genuinely the old behaviour).
//   2. all +1 loaded         -> plain fixed Hadamard, verbatim in the register.
//   3. custom +/-1 loaded    -> applied verbatim.
//
// Build/run:  make test-signreg   (sw/reference_model/Makefile)
#include "kv_cache_engine_ref.hpp"
#include <cstdio>
#include <vector>

using lhsi::KVCacheEngine;
using lhsi::KVCacheEngineInfo;

static int failures = 0;
static void check(bool ok, const char* msg) {
    std::printf("  [%s] %s\n", ok ? "PASS" : "FAIL", msg);
    if (!ok) failures++;
}

int main() {
    const uint32_t D = 64;

    // 1. legacy default (empty) — seed-derived, still has -1s (unchanged behaviour)
    KVCacheEngineInfo li;
    li.vector_dim = D;
    KVCacheEngine legacy(li);
    const std::vector<int8_t>& ls = legacy.sign_flips();
    bool has_neg = false;
    for (int8_t s : ls) if (s == -1) has_neg = true;
    check(ls.size() == D, "legacy: sign register sized to vector_dim");
    check(has_neg, "legacy: empty config -> seed-derived pattern (has -1s, unchanged)");

    // 2. all +1 loaded == plain fixed Hadamard, verbatim
    KVCacheEngineInfo fi;
    fi.vector_dim = D;
    fi.sign_flips = std::vector<int8_t>(D, 1);
    KVCacheEngine fixed(fi);
    bool all_one = true;
    for (int8_t s : fixed.sign_flips()) if (s != 1) all_one = false;
    check(all_one, "all +1 loaded verbatim (plain fixed Hadamard)");

    // 3. custom pattern loaded verbatim
    std::vector<int8_t> pat(D);
    for (uint32_t i = 0; i < D; i++) pat[i] = (i % 2 == 0) ? 1 : -1;
    KVCacheEngineInfo ci;
    ci.vector_dim = D;
    ci.sign_flips = pat;
    KVCacheEngine cust(ci);
    check(cust.sign_flips() == pat, "custom +/-1 pattern loaded verbatim");

    std::printf(failures ? "\nSIGN REGISTER: %d FAIL\n" : "\nSIGN REGISTER: all pass\n", failures);
    return failures;
}
