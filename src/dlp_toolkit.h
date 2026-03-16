// Core library entry point for dlp_toolkit
#include <iostream>
#include <string>
#include <unordered_map>
#include <bitset>
#include <assert.h>

#ifdef TRACY_ENABLE
	#include "tracy/Tracy.hpp"
	// Redefining new/delete with TracyAlloc makes memory profiling visible in Tracy GUI
	void* operator new(size_t count) {
		auto ptr = malloc(count);
		TracyAlloc(ptr, count);
		return ptr;
	}

	void operator delete(void* ptr) noexcept {
		TracyFree(ptr);
		free(ptr);
	}
#endif

using u64 = uint64_t;
using i64 = int64_t;
using u128 = __uint128_t;
using i128 = __int128_t;

u64 addmod(u64 a, u64 b, u64 m) {
	#ifdef TRACY_ENABLE
		ZoneScoped;
	#endif	
	u128 c = (u128)a + b;
	return c%m;
}

u64 mulmod(u64 a, u64 b, u64 m) {
	#ifdef TRACY_ENABLE
		ZoneScoped;
	#endif	
	u128 c = (u128)a * b;
	return c%m;
}
// NOTE:
// PROFILING
// mulmod and addmod both require 128-bit division to reduce modluo m. This is very costly
// and will likely will need some changes. Be on the look out.

// My vesrion of the Fast powering Algorithm to calculate a^b (mod m)
u64 powmod_BAD(u64 a, u64 b, u64 m) {
	#ifdef TRACY_ENABLE
		ZoneScoped;
	#endif	
	std::string b_binary = std::bitset<64>(b).to_string();
	// investigate replacing with 64 - __builtin_clzll(b)
	int max_square_power = 64 - static_cast<int>(b_binary.find_first_of("1"));

	u64 aa =  mulmod(a, a, m);
	std::unordered_map<int, u64> iterative_squares = {
		{0, a},
		{1, aa}
	};

	for (int i = 2; i < max_square_power; i++){
		u64 prev_val = iterative_squares[i-1];
		iterative_squares[i] = mulmod(prev_val, prev_val, m);
	}
	
	int start = 63;
	int end = 63 - max_square_power;
	u64 val = 1;
	for (int i = start; i > end; i--) {
		if (b_binary[i] == '1') {
			val = mulmod(val, iterative_squares[63-i], m);
			#ifdef DEBUG
				std::cout << "i = " << i << "\n";
				std::cout << "desired val: " << iterative_squares[63-i] <<"\n";
				std::cout << "val: " << val << "\n";
			#endif

		}
	}

	#ifdef DEBUG
		// for (const auto& [key, value] : iterative_squares) {
		// 	std::cout << std::format("Key: {}\nValue: {}\n", key, value);
		// }
		// std::cout << std::endl;
	#endif
	
	return val;
}

// Proper implementation of fast-powering algorithm
u64 powmod(u64 a, u64 b, u64 m) {
	#ifdef TRACY_ENABLE
		ZoneScoped;
	#endif	
	u64 res = 1;

	if ( m == 1 ) return 0;
	if ( b == 0 ) return 1;

	while (b > 0) {
		if (b & 1) {
			res = mulmod(res, a, m);
		}

		a = mulmod(a, a, m);
		b >>= 1;
	}

	return res;
}

struct gcd_ctx {
	u64 g;   // gcd value
	i64 u,v; // extended gcd u,v values
};

gcd_ctx gcd(u64 a_in, u64 b_in) {
	#ifdef TRACY_ENABLE
		ZoneScoped;
	#endif
	// assume a > b
	i128 a = a_in;
	i128 b = b_in;
	i128 u = 1, v = 0;
	i128 x = 0, y = 1;

	while (b > 0) {
		i128 q = a / b;
		i128 t = a % b;
		a = b; b = t;

		i128 next_u = u - q * x;
		u = x; x = next_u;

		i128 next_v = v - q * y;
		v = y; y = next_v;
	}


	return gcd_ctx{.g = (u64)a, .u = (i64)u, .v = (i64)v};
}

u64 modinv(u64 a, u64 m) {
	#ifdef TRACY_ENABLE
		ZoneScoped;
	#endif
	gcd_ctx res = gcd(a, m);
	if (res.g != 1) {
		std::cout << "ERROR: a and m are not coprime. inverse does not exist";
		return 0;
	}
	#ifdef DEBUG
		std::cout << std::format("u = {}\nv = {}\ng = {}", res.u, res.v, res.g) << std::endl;
	#endif
	// in C++ % allows negatives so -17 % 47 = -17, not 30
	// Have to do trick: (x % m + m) % m
	u64 a_inv = (u64)((res.u % (i64)m + (i64)m) % (i64)m);

	return a_inv;
}

u64 bsgs_naive(u64 g, u64 order, u64 h, u64 m) {
	#ifdef TRACY_ENABLE
		ZoneScoped;
	#endif
	// assuming order = p - 1
	// u64 m = order + 1;
	u64 n = std::ceil(std::sqrt(order));
	std::vector<u64> l1;
	{
		#ifdef TRACY_ENABLE
			ZoneScopedN("l1 resize");
		#endif
		l1.resize(n+1);
	}

 	// std::vector<u64> l2;
	// {
	// 	#ifdef TRACY_ENABLE
	// 		ZoneScopedN("l2 resize");
	// 	#endif
	// 	l2.resize(n+1);
	// }
	{
		#ifdef TRACY_ENABLE
			ZoneScopedN("construct l1")
		#endif

		// constructing list 1
		l1.at(0) = 1;
		l1.at(1) = g;
		for (int i = 2; i <= n; i++) {
			u64 gi = powmod(g, i, m);
			l1.at(i) = gi;
		}
	}
	// computing list 2 and searching in list 1
	// l2.at(0) = h;
	u64 g_ninv = modinv(powmod(g, n, m), m);
	u64 gj = 1;
	
	for (int j = 0; j <= n; j++) {
		//
		// u64 hg_jninv = mulmod(h, powmod(g_ninv, j, m), m);
		u64 hg_jninv = mulmod(h, gj, m);
		{
			#ifdef TRACY_ENABLE
				ZoneScopedN("std::find l2 in l1")
			#endif
			auto it = std::find(l1.begin(), l1.end(), hg_jninv);
			if (it != l1.end()){
				auto idx = it - l1.begin();
				return idx + j * n;
			}
		}
		gj = mulmod(gj, g_ninv, m);
	}

	return 0;

}


u64 bsgs(u64 g, u64 order, u64 h, u64 m) {
	#ifdef TRACY_ENABLE
		ZoneScoped;
	#endif
	// assuming order = p - 1
	// u64 m = order + 1;
		u64 n = std::ceil(std::sqrt(order));

		std::unordered_map<u64, int> l1;
		l1.reserve(n+1);
		// std::vector<u64> l2;
		{	
		#ifdef TRACY_ENABLE
			ZoneScopedN("constructing l1 @ hashmap");
		#endif
		// constructing list 1
		l1[1] = 0;
		l1[g] = 1;
		for (int i = 2; i <= n; i++) {
			u64 gi = powmod(g, i, m);
			l1[gi] = i;
		}
	}


	// calculate values of list 2 and find match
	u64 g_ninv = modinv(powmod(g, n, m), m);
	u64 gj = 1;
	for (int j = 0; j <= n; j++) {
		u64 hgj = mulmod(h, gj, m);
		{
			#ifdef TRACY_ENABLE
				ZoneScopedN("hashmap check l1 keys for l2");
			#endif
			if (l1.count(hgj)) {
				return l1[hgj] + j * n;
			}
		}
		gj = mulmod(gj, g_ninv, m);
	}

	return 0;
}

// input: list of congruences x = a_i (mod m_i)
// output: solution x mod (m_1*m_2*...*m_n) that satisfies each x = a_i (mod m_i)
u64 crt(std::vector<u64> as, std::vector<u64> ms) {
	assert(as.size() == ms.size());
	size_t n = as.size();

	u64 M = 1;
	for (u64 m : ms) {
		M*=m;
	}

	// std::cout << "M = " << M << "\n";

	u64 total = 0;
	for (int i = 0; i < n; i++) {
		u64 ai = as[i];
		u64 mi = ms[i];

		u64 bi = M/mi;
		// std::cout << "bi = " << bi << "\nmi = " << mi << "\n";
		u64 bi_inv = modinv(bi, mi);
		
		total = addmod(total, mulmod(ai, mulmod(bi, bi_inv, M), M), M);
	}

	return total;
}

struct prime_power {
	u64 pi;
	u64 ei;
};

u64 pohlid_hellman(u64 g, u64 h, u64 p, std::vector<prime_power> p_factors) {}
