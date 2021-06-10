#include "xorshift.h"

XORShift128p::XORShift128p() {
  for (int i = 0; i < 2; i++)
    s[i] = uint64_t(random_device{}());
}

XORShift128p::~XORShift128p() {}

uint64_t XORShift128p::rotl(const uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}

uint64_t XORShift128p::next() {
  const uint64_t s0 = s[0];
  uint64_t s1 = s[1];
  const uint64_t result = s0 + s1;

  s1 ^= s0;
  s[0] = rotl(s0, 55) ^ s1 ^ (s1 << 14);
  s[1] = rotl(s1, 36);
  return result;
}

void XORShift128p::jump() {
  static const uint64_t JUMP[] = {0xbeac0467eba5facb, 0xd86b048b86aa9922};
  uint64_t s0 = 0;
  uint64_t s1 = 0;
  for (size_t i = 0; i < sizeof JUMP / sizeof *JUMP; i++) {
    for (size_t b = 0; b < 64; b++) {
      if (JUMP[i] & 1ULL << b) {
        s0 ^= s[0];
        s1 ^= s[1];
      }
      next();
    }
  }
  s[0] = s0;
  s[1] = s1;
}

XORShift1024m::XORShift1024m() : p(0) {
  for (int i = 0; i < 16; i++)
    s[i] = uint64_t(random_device{}());
}

XORShift1024m::~XORShift1024m() {}

uint64_t XORShift1024m::next() {
  const uint64_t s0 = s[p];
  uint64_t s1 = s[p = (p + 1) & 15];
  s1 ^= s1 << 31;
  s[p] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30);
  return s[p] * UINT64_C(1181783497276652981);
}

void XORShift1024m::jump() {
  static const uint64_t JUMP[] = {
      0x84242f96eca9c41d, 0x5b34a39f070b5837, 0x4489affce4f31a1e,
      0x2ffeeb0a48316f40, 0xdc2d9891fe68c022, 0x3659132bb12fea70,
      0xaac17d8efa43cab8, 0xc4cb815590989b13, 0x5ee975283d71c93b,
      0x691548c86c1bd540, 0x7910c41d10a1e6a5, 0x0b5fc64563b3e2a8,
      0x047f7684e9fc949d, 0xb99181f2d8f685ca, 0x284600e3f30e38c3};

  uint64_t t[16] = {0};
  for (size_t i = 0; i < sizeof JUMP / sizeof *JUMP; i++) {
    for (size_t b = 0; b < 64; b++) {
      if (JUMP[i] & UINT64_C(1) << b) {
        for (size_t j = 0; j < 16; j++) {
          t[j] ^= s[(j + p) & 15];
        }
      }
      next();
    }
  }
  for (size_t j = 0; j < 16; j++)
    s[(j + p) & 15] = t[j];
}
