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
static void
#if defined(AESNI)
cn_slow_hash_aesni
#else
cn_slow_hash_noaesni
#endif
(void *restrict context, const void *restrict data, size_t length, void *restrict hash)
{
#define ctx ((struct cn_ctx *) context)
  ALIGNED_DECL(uint8_t ExpandedKey[256], 16);
  size_t i;
  __m128i *longoutput, *expkey, *xmminput, b_x;
  ALIGNED_DECL(uint64_t a[2], 16);
  hash_process(&ctx->state.hs, (const uint8_t*) data, length);

  memcpy(ctx->text, ctx->state.init, INIT_SIZE_BYTE);
#if defined(AESNI)
  memcpy(ExpandedKey, ctx->state.hs.b, AES_KEY_SIZE);
  ExpandAESKey256(ExpandedKey);
#else
  ctx->aes_ctx = oaes_alloc();
  oaes_key_import_data(ctx->aes_ctx, ctx->state.hs.b, AES_KEY_SIZE);
  memcpy(ExpandedKey, ctx->aes_ctx->key->exp_data, ctx->aes_ctx->key->exp_data_len);
#endif

  longoutput = (__m128i *) ctx->long_state;
  expkey = (__m128i *) ExpandedKey;
  xmminput = (__m128i *) ctx->text;

  //for (i = 0; likely(i < MEMORY); i += INIT_SIZE_BYTE)
  //    aesni_parallel_noxor(&ctx->long_state[i], ctx->text, ExpandedKey);

  for (i = 0; likely(i < MEMORY); i += INIT_SIZE_BYTE)
  {
#if defined(AESNI)
    for(size_t j = 0; j < 10; j++)
    {
      xmminput[0] = _mm_aesenc_si128(xmminput[0], expkey[j]);
      xmminput[1] = _mm_aesenc_si128(xmminput[1], expkey[j]);
      xmminput[2] = _mm_aesenc_si128(xmminput[2], expkey[j]);
      xmminput[3] = _mm_aesenc_si128(xmminput[3], expkey[j]);
      xmminput[4] = _mm_aesenc_si128(xmminput[4], expkey[j]);
      xmminput[5] = _mm_aesenc_si128(xmminput[5], expkey[j]);
      xmminput[6] = _mm_aesenc_si128(xmminput[6], expkey[j]);
      xmminput[7] = _mm_aesenc_si128(xmminput[7], expkey[j]);
    }
#else
    aesb_pseudo_round((uint8_t *) &xmminput[0], (uint8_t *) &xmminput[0], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[1], (uint8_t *) &xmminput[1], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[2], (uint8_t *) &xmminput[2], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[3], (uint8_t *) &xmminput[3], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[4], (uint8_t *) &xmminput[4], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[5], (uint8_t *) &xmminput[5], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[6], (uint8_t *) &xmminput[6], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[7], (uint8_t *) &xmminput[7], (uint8_t *) expkey);
#endif
    _mm_store_si128(&(longoutput[(i >> 4)]), xmminput[0]);
    _mm_store_si128(&(longoutput[(i >> 4) + 1]), xmminput[1]);
    _mm_store_si128(&(longoutput[(i >> 4) + 2]), xmminput[2]);
    _mm_store_si128(&(longoutput[(i >> 4) + 3]), xmminput[3]);
    _mm_store_si128(&(longoutput[(i >> 4) + 4]), xmminput[4]);
    _mm_store_si128(&(longoutput[(i >> 4) + 5]), xmminput[5]);
    _mm_store_si128(&(longoutput[(i >> 4) + 6]), xmminput[6]);
    _mm_store_si128(&(longoutput[(i >> 4) + 7]), xmminput[7]);
  }

  for (i = 0; i < 2; i++)
  {
    ctx->a[i] = ((uint64_t *)ctx->state.k)[i] ^  ((uint64_t *)ctx->state.k)[i+4];
    ctx->b[i] = ((uint64_t *)ctx->state.k)[i+2] ^  ((uint64_t *)ctx->state.k)[i+6];
  }

  b_x = _mm_load_si128((__m128i *)ctx->b);
  a[0] = ctx->a[0];
  a[1] = ctx->a[1];

  for(i = 0; likely(i < 0x80000); i++)
  {
    __m128i c_x = _mm_load_si128((__m128i *)&ctx->long_state[a[0] & 0x1FFFF0]);
    __m128i a_x = _mm_load_si128((__m128i *)a);
    ALIGNED_DECL(uint64_t c[2], 16);
    ALIGNED_DECL(uint64_t b[2], 16);
    uint64_t *nextblock, *dst;

#if defined(AESNI)
    c_x = _mm_aesenc_si128(c_x, a_x);
#else
    aesb_single_round((uint8_t *) &c_x, (uint8_t *) &c_x, (uint8_t *) &a_x);
#endif

    _mm_store_si128((__m128i *)c, c_x);
    //__builtin_prefetch(&ctx->long_state[c[0] & 0x1FFFF0], 0, 1);

    b_x = _mm_xor_si128(b_x, c_x);
    _mm_store_si128((__m128i *)&ctx->long_state[a[0] & 0x1FFFF0], b_x);

    nextblock = (uint64_t *)&ctx->long_state[c[0] & 0x1FFFF0];
    b[0] = nextblock[0];
    b[1] = nextblock[1];

    {
      uint64_t hi, lo;
      // hi,lo = 64bit x 64bit multiply of c[0] and b[0]

#if defined(__GNUC__) && defined(__x86_64__)
      __asm__("mulq %3\n\t"
        : "=d" (hi),
        "=a" (lo)
        : "%a" (c[0]),
        "rm" (b[0])
        : "cc" );
#else
      lo = mul128(c[0], b[0], &hi);
#endif

      a[0] += hi;
      a[1] += lo;
    }
    dst = (uint64_t *) &ctx->long_state[c[0] & 0x1FFFF0];
    dst[0] = a[0];
    dst[1] = a[1];

    a[0] ^= b[0];
    a[1] ^= b[1];
    b_x = c_x;
    //__builtin_prefetch(&ctx->long_state[a[0] & 0x1FFFF0], 0, 3);
  }

  memcpy(ctx->text, ctx->state.init, INIT_SIZE_BYTE);
#if defined(AESNI)
  memcpy(ExpandedKey, &ctx->state.hs.b[32], AES_KEY_SIZE);
  ExpandAESKey256(ExpandedKey);
#else
  oaes_key_import_data(ctx->aes_ctx, &ctx->state.hs.b[32], AES_KEY_SIZE);
  memcpy(ExpandedKey, ctx->aes_ctx->key->exp_data, ctx->aes_ctx->key->exp_data_len);
#endif

  //for (i = 0; likely(i < MEMORY); i += INIT_SIZE_BYTE)
  //    aesni_parallel_xor(&ctx->text, ExpandedKey, &ctx->long_state[i]);

  for (i = 0; likely(i < MEMORY); i += INIT_SIZE_BYTE)
  {
    xmminput[0] = _mm_xor_si128(longoutput[(i >> 4)], xmminput[0]);
    xmminput[1] = _mm_xor_si128(longoutput[(i >> 4) + 1], xmminput[1]);
    xmminput[2] = _mm_xor_si128(longoutput[(i >> 4) + 2], xmminput[2]);
    xmminput[3] = _mm_xor_si128(longoutput[(i >> 4) + 3], xmminput[3]);
    xmminput[4] = _mm_xor_si128(longoutput[(i >> 4) + 4], xmminput[4]);
    xmminput[5] = _mm_xor_si128(longoutput[(i >> 4) + 5], xmminput[5]);
    xmminput[6] = _mm_xor_si128(longoutput[(i >> 4) + 6], xmminput[6]);
    xmminput[7] = _mm_xor_si128(longoutput[(i >> 4) + 7], xmminput[7]);

#if defined(AESNI)
    for(size_t j = 0; j < 10; j++)
    {
      xmminput[0] = _mm_aesenc_si128(xmminput[0], expkey[j]);
      xmminput[1] = _mm_aesenc_si128(xmminput[1], expkey[j]);
      xmminput[2] = _mm_aesenc_si128(xmminput[2], expkey[j]);
      xmminput[3] = _mm_aesenc_si128(xmminput[3], expkey[j]);
      xmminput[4] = _mm_aesenc_si128(xmminput[4], expkey[j]);
      xmminput[5] = _mm_aesenc_si128(xmminput[5], expkey[j]);
      xmminput[6] = _mm_aesenc_si128(xmminput[6], expkey[j]);
      xmminput[7] = _mm_aesenc_si128(xmminput[7], expkey[j]);
    }
#else
    aesb_pseudo_round((uint8_t *) &xmminput[0], (uint8_t *) &xmminput[0], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[1], (uint8_t *) &xmminput[1], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[2], (uint8_t *) &xmminput[2], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[3], (uint8_t *) &xmminput[3], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[4], (uint8_t *) &xmminput[4], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[5], (uint8_t *) &xmminput[5], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[6], (uint8_t *) &xmminput[6], (uint8_t *) expkey);
    aesb_pseudo_round((uint8_t *) &xmminput[7], (uint8_t *) &xmminput[7], (uint8_t *) expkey);
#endif

  }

#if !defined(AESNI)
  oaes_free((OAES_CTX **) &ctx->aes_ctx);
#endif

  memcpy(ctx->state.init, ctx->text, INIT_SIZE_BYTE);
  hash_permutation(&ctx->state.hs);
  extra_hashes[ctx->state.hs.b[0] & 3](&ctx->state, 200, hash);
}
