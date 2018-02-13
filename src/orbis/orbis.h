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
#pragma once

#include <cstddef>
#include <limits>
#include <mutex>
#include <type_traits>
#include <vector>

#include <CryptoTypes.h>

#include "generic-ops.h"
#include "hash.h"

namespace Crypto {

  extern "C" {
#include "random.h"
  }

  extern std::mutex random_lock;

struct EllipticCurvePoint {
  uint8_t data[32];
};

struct EllipticCurveScalar {
  uint8_t data[32];
};

  class orbis_ops {
    orbis_ops();
    orbis_ops(const orbis_ops &);
    void operator=(const orbis_ops &);
    ~orbis_ops();

    static void generate_keys(PublicKey &, SecretKey &);
    friend void generate_keys(PublicKey &, SecretKey &);
    static bool check_key(const PublicKey &);
    friend bool check_key(const PublicKey &);
    static bool secret_key_to_public_key(const SecretKey &, PublicKey &);
    friend bool secret_key_to_public_key(const SecretKey &, PublicKey &);
    static bool generate_key_derivation(const PublicKey &, const SecretKey &, KeyDerivation &);
    friend bool generate_key_derivation(const PublicKey &, const SecretKey &, KeyDerivation &);
    static bool derive_public_key(const KeyDerivation &, size_t, const PublicKey &, PublicKey &);
    friend bool derive_public_key(const KeyDerivation &, size_t, const PublicKey &, PublicKey &);
    friend bool derive_public_key(const KeyDerivation &, size_t, const PublicKey &, const uint8_t*, size_t, PublicKey &);
    static bool derive_public_key(const KeyDerivation &, size_t, const PublicKey &, const uint8_t*, size_t, PublicKey &);
    //hack for pg
    static bool underive_public_key_and_get_scalar(const KeyDerivation &, std::size_t, const PublicKey &, PublicKey &, EllipticCurveScalar &);
    friend bool underive_public_key_and_get_scalar(const KeyDerivation &, std::size_t, const PublicKey &, PublicKey &, EllipticCurveScalar &);
    static void generate_incomplete_key_image(const PublicKey &, EllipticCurvePoint &);
    friend void generate_incomplete_key_image(const PublicKey &, EllipticCurvePoint &);
    //
    static void derive_secret_key(const KeyDerivation &, size_t, const SecretKey &, SecretKey &);
    friend void derive_secret_key(const KeyDerivation &, size_t, const SecretKey &, SecretKey &);
    static void derive_secret_key(const KeyDerivation &, size_t, const SecretKey &, const uint8_t*, size_t, SecretKey &);
    friend void derive_secret_key(const KeyDerivation &, size_t, const SecretKey &, const uint8_t*, size_t, SecretKey &);
    static bool underive_public_key(const KeyDerivation &, size_t, const PublicKey &, PublicKey &);
    friend bool underive_public_key(const KeyDerivation &, size_t, const PublicKey &, PublicKey &);
    static bool underive_public_key(const KeyDerivation &, size_t, const PublicKey &, const uint8_t*, size_t, PublicKey &);
    friend bool underive_public_key(const KeyDerivation &, size_t, const PublicKey &, const uint8_t*, size_t, PublicKey &);
    static void generate_signature(const Hash &, const PublicKey &, const SecretKey &, Signature &);
    friend void generate_signature(const Hash &, const PublicKey &, const SecretKey &, Signature &);
    static bool check_signature(const Hash &, const PublicKey &, const Signature &);
    friend bool check_signature(const Hash &, const PublicKey &, const Signature &);
    static void generate_key_image(const PublicKey &, const SecretKey &, KeyImage &);
    friend void generate_key_image(const PublicKey &, const SecretKey &, KeyImage &);
    static void hash_data_to_ec(const uint8_t*, std::size_t, PublicKey&);
    friend void hash_data_to_ec(const uint8_t*, std::size_t, PublicKey&);
    static void generate_ring_signature(const Hash &, const KeyImage &,
      const PublicKey *const *, size_t, const SecretKey &, size_t, Signature *);
    friend void generate_ring_signature(const Hash &, const KeyImage &,
      const PublicKey *const *, size_t, const SecretKey &, size_t, Signature *);
    static bool check_ring_signature(const Hash &, const KeyImage &,
      const PublicKey *const *, size_t, const Signature *);
    friend bool check_ring_signature(const Hash &, const KeyImage &,
      const PublicKey *const *, size_t, const Signature *);
  };

  /* Generate a value filled with random bytes.
   */
  template<typename T>
  typename std::enable_if<std::is_pod<T>::value, T>::type rand() {
    typename std::remove_cv<T>::type res;
    std::lock_guard<std::mutex> lock(random_lock);
    generate_random_bytes(sizeof(T), &res);
    return res;
  }

  /* Random number engine based on Crypto::rand()
   */
  template <typename T>
  class random_engine {
  public:
    typedef T result_type;

#ifdef __clang__
    constexpr static T min() {
      return (std::numeric_limits<T>::min)();
    }

    constexpr static T max() {
      return (std::numeric_limits<T>::max)();
    }
#else
    static T(min)() {
      return (std::numeric_limits<T>::min)();
    }

    static T(max)() {
      return (std::numeric_limits<T>::max)();
    }
#endif
    typename std::enable_if<std::is_unsigned<T>::value, T>::type operator()() {
      return rand<T>();
    }
  };

  /* Generate a new key pair
   */
  inline void generate_keys(PublicKey &pub, SecretKey &sec) {
    orbis_ops::generate_keys(pub, sec);
  }

  /* Check a public key. Returns true if it is valid, false otherwise.
   */
  inline bool check_key(const PublicKey &key) {
    return orbis_ops::check_key(key);
  }

  /* Checks a private key and computes the corresponding public key.
   */
  inline bool secret_key_to_public_key(const SecretKey &sec, PublicKey &pub) {
    return orbis_ops::secret_key_to_public_key(sec, pub);
  }

  /* To generate an ephemeral key used to send money to:
   * * The sender generates a new key pair, which becomes the transaction key. The public transaction key is included in "extra" field.
   * * Both the sender and the receiver generate key derivation from the transaction key and the receivers' "view" key.
   * * The sender uses key derivation, the output index, and the receivers' "spend" key to derive an ephemeral public key.
   * * The receiver can either derive the public key (to check that the transaction is addressed to him) or the private key (to spend the money).
   */
  inline bool generate_key_derivation(const PublicKey &key1, const SecretKey &key2, KeyDerivation &derivation) {
    return orbis_ops::generate_key_derivation(key1, key2, derivation);
  }

  inline bool derive_public_key(const KeyDerivation &derivation, size_t output_index,
    const PublicKey &base, const uint8_t* prefix, size_t prefixLength, PublicKey &derived_key) {
    return orbis_ops::derive_public_key(derivation, output_index, base, prefix, prefixLength, derived_key);
  }

  inline bool derive_public_key(const KeyDerivation &derivation, size_t output_index,
    const PublicKey &base, PublicKey &derived_key) {
    return orbis_ops::derive_public_key(derivation, output_index, base, derived_key);
  }


  inline bool underive_public_key_and_get_scalar(const KeyDerivation &derivation, std::size_t output_index,
    const PublicKey &derived_key, PublicKey &base, EllipticCurveScalar &hashed_derivation) {
    return orbis_ops::underive_public_key_and_get_scalar(derivation, output_index, derived_key, base, hashed_derivation);
  }
  
  inline void derive_secret_key(const KeyDerivation &derivation, std::size_t output_index,
    const SecretKey &base, const uint8_t* prefix, size_t prefixLength, SecretKey &derived_key) {
    orbis_ops::derive_secret_key(derivation, output_index, base, prefix, prefixLength, derived_key);
  }

  inline void derive_secret_key(const KeyDerivation &derivation, std::size_t output_index,
    const SecretKey &base, SecretKey &derived_key) {
    orbis_ops::derive_secret_key(derivation, output_index, base, derived_key);
  }


  /* Inverse function of derive_public_key. It can be used by the receiver to find which "spend" key was used to generate a transaction. This may be useful if the receiver used multiple addresses which only differ in "spend" key.
   */
  inline bool underive_public_key(const KeyDerivation &derivation, size_t output_index,
    const PublicKey &derived_key, const uint8_t* prefix, size_t prefixLength, PublicKey &base) {
    return orbis_ops::underive_public_key(derivation, output_index, derived_key, prefix, prefixLength, base);
  }

  inline bool underive_public_key(const KeyDerivation &derivation, size_t output_index,
    const PublicKey &derived_key, PublicKey &base) {
    return orbis_ops::underive_public_key(derivation, output_index, derived_key, base);
  }

  /* Generation and checking of a standard signature.
   */
  inline void generate_signature(const Hash &prefix_hash, const PublicKey &pub, const SecretKey &sec, Signature &sig) {
    orbis_ops::generate_signature(prefix_hash, pub, sec, sig);
  }
  inline bool check_signature(const Hash &prefix_hash, const PublicKey &pub, const Signature &sig) {
    return orbis_ops::check_signature(prefix_hash, pub, sig);
  }

  /* To send money to a key:
   * * The sender generates an ephemeral key and includes it in transaction output.
   * * To spend the money, the receiver generates a key image from it.
   * * Then he selects a bunch of outputs, including the one he spends, and uses them to generate a ring signature.
   * To check the signature, it is necessary to collect all the keys that were used to generate it. To detect double spends, it is necessary to check that each key image is used at most once.
   */
  inline void generate_key_image(const PublicKey &pub, const SecretKey &sec, KeyImage &image) {
    orbis_ops::generate_key_image(pub, sec, image);
  }

  inline void hash_data_to_ec(const uint8_t* data, std::size_t len, PublicKey& key) {
    orbis_ops::hash_data_to_ec(data, len, key);
  }

  inline void generate_ring_signature(const Hash &prefix_hash, const KeyImage &image,
    const PublicKey *const *pubs, std::size_t pubs_count,
    const SecretKey &sec, std::size_t sec_index,
    Signature *sig) {
    orbis_ops::generate_ring_signature(prefix_hash, image, pubs, pubs_count, sec, sec_index, sig);
  }
  inline bool check_ring_signature(const Hash &prefix_hash, const KeyImage &image,
    const PublicKey *const *pubs, size_t pubs_count,
    const Signature *sig) {
    return orbis_ops::check_ring_signature(prefix_hash, image, pubs, pubs_count, sig);
  }

  /* Variants with vector<const PublicKey *> parameters.
   */
  inline void generate_ring_signature(const Hash &prefix_hash, const KeyImage &image,
    const std::vector<const PublicKey *> &pubs,
    const SecretKey &sec, size_t sec_index,
    Signature *sig) {
    generate_ring_signature(prefix_hash, image, pubs.data(), pubs.size(), sec, sec_index, sig);
  }
  inline bool check_ring_signature(const Hash &prefix_hash, const KeyImage &image,
    const std::vector<const PublicKey *> &pubs,
    const Signature *sig) {
    return check_ring_signature(prefix_hash, image, pubs.data(), pubs.size(), sig);
  }

}

CRYPTO_MAKE_HASHABLE(PublicKey)
CRYPTO_MAKE_HASHABLE(KeyImage)
CRYPTO_MAKE_COMPARABLE(Signature)
CRYPTO_MAKE_COMPARABLE(SecretKey)
