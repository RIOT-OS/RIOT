//#include <Arduino.h>
#include "Ed25519.h"

// imported from SUPERCOP by Daniel J. Bernstein which is public domain

typedef uint64_t crypto_uint64;
typedef int64_t crypto_int64;

typedef int32_t crypto_int32;

typedef crypto_int32 fe[10];

static crypto_uint64 load_3(const unsigned char *in);

static crypto_uint64 load_4(const unsigned char *in);

void fe_sub(fe h, fe f, fe g);

void fe_frombytes(fe h, const unsigned char *s);

void fe_1(fe h);

void fe_add(fe h, const fe f, const fe g);

void fe_sq(fe h, const fe f);

void fe_mul(fe h, const fe f, const fe g);

void fe_invert(fe out, const fe z);

void fe_tobytes(unsigned char *s, const fe h);

// Computes the encoded point scalar * B (B = Ed25519 base point), for example on a
// crypto accelerator. scalar is 32 bytes little-endian, already reduced mod q.
// Returns false to make sign() fall back to its own software multiplication.
typedef bool (*XEdDSABaseMulFn)(void *ctx, uint8_t point[32], const uint8_t scalar[32]);

class XEdDSA : public Ed25519
{
public:
    static void priv_curve_to_ed_keys(uint8_t *curve_privkey, uint8_t *ed_privkey, uint8_t *ed_pubkey);
    // baseMul, if given, computes R = r * B; the signature is byte-identical either way.
    static void sign(uint8_t signature[64], const uint8_t privateKey[32],
                   const uint8_t publicKey[32], const void *message, size_t len,
                   XEdDSABaseMulFn baseMul = nullptr, void *baseMulCtx = nullptr);
private:
        static void deriveKeys(SHA512 *hash, limb_t *a, const uint8_t privateKey[32]);


};
