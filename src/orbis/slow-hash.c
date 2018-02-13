// Copyright (c) 2018 The Orbis developers
// ======================================================================
//              ===== Spell of Creation =====
// script inspired by the CNC code, on February 11, 2018
// 
// ======================================================================
/*                ,     
			     d 
                 :8;        oo 
                ,888     od88 Y 
                " b     "`88888  ,. 
                  :    88888888o. 
                   Y.   `8888888bo 
                    "bo  d888888888oooooo,. 
                      "Yo8:8888888888888Yd8. 
                        `8b;Y88888888888888' 
                        ,888d"Y888888888888.  , 
                        P88Y8b.888888888888b.         `b 
                        :;  "8888888888888888P8b       d; 
                         .   Y88888888888888bo   ",o ,d8 
                              `"8888888888888888888888",oo. 
                               :888888888888888888888P""   ` 
                               `88888888888888888888oooooo. : 
                            ;  ,888888Y888888888888888888888" 
                            `'`P""8.`8;`888888888888888888888o. 
                              `    ` :;  `888888888888888"""8P"o. 
                                 ,  '`8    Y88888888888888;  :b Yb 
                                  8.  :.    `Y8888888888888; dP  `8 
                                ,8d8    "     `888888888888d      Y; 
                                :888d8;        88888888888Pb       ; 
                                :888' `   o   d888888888888;      :' 
                               oo888bo  ,."8888888888888888;    ' ' 
                               8888888888888888888888888888; 
                       ,.`88booo`Y888888888888888888888888P' 
                           :888888888888888888888888888888' 
                   ,ooooood888888888888888888888888888888' 
                  ""888888888888888888888888888888888888; 
             ,oooooo888888888888888888888888888888888888' 
               "88888888888888888888888888888888888888P 
       ,oo bo ooo88888888888888888888888888888888888Y8 
     ."8P88d888888888888888888888888888888888888888"`"o. 
      oo888888888888888888888888888888888888888888"    8 
     d88Y8888888888888888888888888888888888888888' ooo8bYooooo. 
    ,""o888888888888888888888888888888888P":888888888888888888b 
    `   ,d88888888888888888888888888888"'  :888888888888888bod8 
      ,88888888888888888888888888888"      `d8888888888888o`88"b 
    ,88888888888888888888888888""            ,88888' 88  Y8b 
    " ,8888888888888888888""        ,;       88' ;   `8'  P 
     d8888888888888888888boo8888888P"         :.     ` 
    d888888888888888888888888888888boooo 
   :"Y888888888888P':88888"""8P"88888P' 
   ` :88888888888'   88""  ,dP' :888888. 
    ,88888888888'          '`  ,88,8b d" 
    d88888888888               dP"`888' 
    Y :888888888;                   8' 
      :8888888888.                 ` 
      :888888888888oo                        ,ooooo 
      :8888888888888o              ,o   oo d88888oooo. 
       ' :888888888888888booooood888888888888888888888bo.  -hrr- 
          Y88888888888888888888888888888888888"""888888o. 
           "`"Y8888888888888888888888888""""'     `"88888b. 
               "888"Y888888888888888"                Y888.' 
                `"'  `""' `"""""'  "          ,       8888 
                                              :.      8888 
                                           d888d8o    88;` 
                                           Y888888;  d88; 
                                         o88888888,o88P:' 
                                    "ood888888888888"' 
                                ,oo88888""888888888. 
                              ,o8P"b8YP  '`"888888;"b. 
                           ' d8"`o8",P    """""""8P 
                                    `;          d8; 
                                                 8;
// =============================================================
//              ===== it's not an illusion =====
//                  ===== it's real =====
//
// =============================================================
*/                          
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <emmintrin.h>
#include <wmmintrin.h>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <cpuid.h>
#endif

#include "aesb.h"
#include "initializer.h"
#include "Common/int-util.h"
#include "hash-ops.h"
#include "oaes_lib.h"

void (*cn_slow_hash_fp)(void *, const void *, size_t, void *);

void cn_slow_hash_f(void * a, const void * b, size_t c, void * d){
(*cn_slow_hash_fp)(a, b, c, d);
}

#if defined(__GNUC__)
#define likely(x) (__builtin_expect(!!(x), 1))
#define unlikely(x) (__builtin_expect(!!(x), 0))
#else
#define likely(x) (x)
#define unlikely(x) (x)
#define __attribute__(x)
#endif

#if defined(_MSC_VER)
#define restrict
#endif

#define MEMORY         (1 << 21) /* 2 MiB */
#define ITER           (1 << 20)
#define AES_BLOCK_SIZE  16
#define AES_KEY_SIZE    32 /*16*/
#define INIT_SIZE_BLK   8
#define INIT_SIZE_BYTE (INIT_SIZE_BLK * AES_BLOCK_SIZE)	// 128

#pragma pack(push, 1)
union cn_slow_hash_state {
  union hash_state hs;
  struct {
    uint8_t k[64];
    uint8_t init[INIT_SIZE_BYTE];
  };
};
#pragma pack(pop)

#if defined(_MSC_VER)
#define ALIGNED_DATA(x) __declspec(align(x))
#define ALIGNED_DECL(t, x) ALIGNED_DATA(x) t
#elif defined(__GNUC__)
#define ALIGNED_DATA(x) __attribute__((aligned(x)))
#define ALIGNED_DECL(t, x) t ALIGNED_DATA(x)
#endif

struct cn_ctx {
  ALIGNED_DECL(uint8_t long_state[MEMORY], 16);
  ALIGNED_DECL(union cn_slow_hash_state state, 16);
  ALIGNED_DECL(uint8_t text[INIT_SIZE_BYTE], 16);
  ALIGNED_DECL(uint64_t a[AES_BLOCK_SIZE >> 3], 16);
  ALIGNED_DECL(uint64_t b[AES_BLOCK_SIZE >> 3], 16);
  ALIGNED_DECL(uint8_t c[AES_BLOCK_SIZE], 16);
  oaes_ctx* aes_ctx;
};

static_assert(sizeof(struct cn_ctx) == SLOW_HASH_CONTEXT_SIZE, "Invalid structure size");

static inline void ExpandAESKey256_sub1(__m128i *tmp1, __m128i *tmp2)
{
  __m128i tmp4;
  *tmp2 = _mm_shuffle_epi32(*tmp2, 0xFF);
  tmp4 = _mm_slli_si128(*tmp1, 0x04);
  *tmp1 = _mm_xor_si128(*tmp1, tmp4);
  tmp4 = _mm_slli_si128(tmp4, 0x04);
  *tmp1 = _mm_xor_si128(*tmp1, tmp4);
  tmp4 = _mm_slli_si128(tmp4, 0x04);
  *tmp1 = _mm_xor_si128(*tmp1, tmp4);
  *tmp1 = _mm_xor_si128(*tmp1, *tmp2);
}

static inline void ExpandAESKey256_sub2(__m128i *tmp1, __m128i *tmp3)
{
  __m128i tmp2, tmp4;

  tmp4 = _mm_aeskeygenassist_si128(*tmp1, 0x00);
  tmp2 = _mm_shuffle_epi32(tmp4, 0xAA);
  tmp4 = _mm_slli_si128(*tmp3, 0x04);
  *tmp3 = _mm_xor_si128(*tmp3, tmp4);
  tmp4 = _mm_slli_si128(tmp4, 0x04);
  *tmp3 = _mm_xor_si128(*tmp3, tmp4);
  tmp4 = _mm_slli_si128(tmp4, 0x04);
  *tmp3 = _mm_xor_si128(*tmp3, tmp4);
  *tmp3 = _mm_xor_si128(*tmp3, tmp2);
}

// Special thanks to Intel for helping me
// with ExpandAESKey256() and its subroutines
static inline void ExpandAESKey256(uint8_t *keybuf)
{
  __m128i tmp1, tmp2, tmp3, *keys;

  keys = (__m128i *)keybuf;

  tmp1 = _mm_load_si128((__m128i *)keybuf);
  tmp3 = _mm_load_si128((__m128i *)(keybuf+0x10));

  tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x01);
  ExpandAESKey256_sub1(&tmp1, &tmp2);
  keys[2] = tmp1;
  ExpandAESKey256_sub2(&tmp1, &tmp3);
  keys[3] = tmp3;

  tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x02);
  ExpandAESKey256_sub1(&tmp1, &tmp2);
  keys[4] = tmp1;
  ExpandAESKey256_sub2(&tmp1, &tmp3);
  keys[5] = tmp3;

  tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x04);
  ExpandAESKey256_sub1(&tmp1, &tmp2);
  keys[6] = tmp1;
  ExpandAESKey256_sub2(&tmp1, &tmp3);
  keys[7] = tmp3;

  tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x08);
  ExpandAESKey256_sub1(&tmp1, &tmp2);
  keys[8] = tmp1;
  ExpandAESKey256_sub2(&tmp1, &tmp3);
  keys[9] = tmp3;

  tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x10);
  ExpandAESKey256_sub1(&tmp1, &tmp2);
  keys[10] = tmp1;
  ExpandAESKey256_sub2(&tmp1, &tmp3);
  keys[11] = tmp3;

  tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x20);
  ExpandAESKey256_sub1(&tmp1, &tmp2);
  keys[12] = tmp1;
  ExpandAESKey256_sub2(&tmp1, &tmp3);
  keys[13] = tmp3;

  tmp2 = _mm_aeskeygenassist_si128(tmp3, 0x40);
  ExpandAESKey256_sub1(&tmp1, &tmp2);
  keys[14] = tmp1;
}

static void (*const extra_hashes[4])(const void *, size_t, char *) =
{
    hash_extra_blake, hash_extra_groestl, hash_extra_jh, hash_extra_skein
};

#include "slow-hash.inl"
#define AESNI
#include "slow-hash.inl"

INITIALIZER(detect_aes) {
  int ecx;
#if defined(_MSC_VER)
  int cpuinfo[4];
  __cpuid(cpuinfo, 1);
  ecx = cpuinfo[2];
#else
  int a, b, d;
  __cpuid(1, a, b, ecx, d);
#endif
  cn_slow_hash_fp = (ecx & (1 << 25)) ? &cn_slow_hash_aesni : &cn_slow_hash_noaesni;
}
