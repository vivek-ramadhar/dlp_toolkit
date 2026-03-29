// #define DEBUG

#include "../src/dlp_toolkit.h"
#include <bitset>
#include <format>
#include <iostream>
#ifdef TRACY_ENABLE
#include "tracy/Tracy.hpp"
#endif

using u64 = uint64_t;

int main() {
  // TEST powmod(a, b, m)
  u64 a = 3;
  u64 b = 218;
  u64 m = 1000;
  std::string b_binary = std::bitset<64>(b).to_string();
  int max_square_power = 64 - static_cast<int>(b_binary.find_first_of("1"));
  std::cout << std::format("b = {}\nb_10 = {}\nMost significant bit: {}", b,
                           b_binary, max_square_power);
  std::cout << "\n";
  u64 val = powmod(a, b, m);
  std::cout << std::format("{}^{} (mod {}) = {}", a, b, m, val);
  std::cout << std::endl;

  // TEST gcd(a, b)
  a = (1ULL << 63) - 1;
  b = (1ULL << 63) - 2;
  // a = 139024789;
  // b = 93278890;
  gcd_ctx gcd_res = gcd(a, b);
  std::cout << std::format("gcd({}, {}) = {}\nu = {}, v = {}", a, b, gcd_res.g,
                           gcd_res.u, gcd_res.v)
            << std::endl;

  // TEST modinv(a, m)
  u64 p = 104801;
  a = 78467;
  u64 a_inv = modinv(a, p);
  std::cout << std::format("{}^-1 (mod {}) = {}", a, p, a_inv) << "\n";

  // TEST bsgs(u64 g, u64 order, u64 h)
  u64 g = 2;
  m = 999999999989ULL;
  u64 order = m - 1;
  u64 h = 174131877207ULL;
  u64 x;
  u64 x_naive;
  {
#ifdef TRACY_ENABLE
    ZoneScopedN("bsgs@main");
#endif
    // x = bsgs(g, order, h, m);
    x = bsgs(5, 19999999966ULL, 6567690556ULL, 19999999967ULL);
    // x = bsgs(5, 49999999966ULL, 6839120451ULL,  49999999967ULL);
  }

  {
#ifdef TRACY_ENABLE
    ZoneScopedN("bsgs_naive@main");
#endif
    // x_naive = bsgs_naive(g, order, h, m);
    x_naive = bsgs_naive(5, 19999999966ULL, 6567690556ULL,
                         19999999967ULL); // x=6666666655
    // x_naive = bsgs_naive(5, 49999999966ULL, 6839120451ULL,  49999999967ULL);
    // // x=16666666655
  }
  std::cout << std::format("{}^x = {}\nx (naive) = {}\nx = {}", g, h, x_naive,
                           x);
  std::cout << std::endl;

  // TEST crt([a1, a2, ...], [m1, m2,...])

  std::vector<u64> moduli = {3, 7, 16};
  std::vector<u64> results = {2, 3, 4};
  u64 M = 1;

  for (auto m : moduli) {
    M *= m;
  }

  x = crt(results, moduli);

  for (int i = 0; i < moduli.size(); i++) {
    std::cout << std::format("x = {} mod({})\n", results[i], moduli[i]);
  }
  std::cout << "x = " << x << " mod(" << M << ")\n";

  // TEST ph_naive(g, h, p, order_factors)
  g = 23;
  h = 9689;
  p = 11251;
  std::vector<prime_power> N_factors = {prime_power{2, 1}, prime_power{3, 2},
                                        prime_power{5, 4}};

  x = ph_naive(g, h, p, N_factors);
  std::cout << std::format("DLP: {}^x = {} in F_{}\n", g, h, p)
            << std::format("x = {}\n", x);

  // TEST ph(g, h, p , order_factors)

  u64 g_test, h_test;
  g_test = 5448;
  h_test = 6909;
  std::vector<prime_power> N_factors_test = {prime_power{5, 4}};

  u64 g_test1, h_test1, p_test1;
  g_test1 = 3;
  p_test1 = 998244353;
  h_test1 = powmod(3, 123456789, p_test1);
  std::vector<prime_power> N_factors_test1 = {
      prime_power{2, 23}, prime_power{7, 1}, prime_power{17, 1}};

  x = ph(g, h, p, N_factors);
  std::cout << std::format("DLP: {}^x = {} in F_{}\n", g, h, p)
            << std::format("x(ph) = {}\n", x);

  u64 x_ph, x_phfast;
  x_naive = bsgs_naive(g_test1, p_test1 - 1, h_test1, p_test1);
  x = bsgs(g_test1, p_test1 - 1, h_test1, p_test1);
  x_ph = ph_naive(g_test1, h_test1, p_test1, N_factors_test1);
  x_phfast = ph(g_test1, h_test1, p_test1, N_factors_test1);
  std::cout << std::format("DLP: {}^x = {} in F_{}\n", g_test1, h_test1,
                           p_test1)
            << std::format("x(bsgs_naive)      = {}\n", x_naive)
            << std::format("x(bsgs)      = {}\n", x)
            << std::format("x(ph_naive)      = {}\n", x_ph)
            << std::format("x(ph) = {}\n", x_phfast);

#ifdef TRACY_ENABLE
  FrameMark;
#endif
}
