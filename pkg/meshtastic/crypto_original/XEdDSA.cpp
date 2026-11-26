#include "XEdDSA.h"
#include "utility/LimbUtil.h"
#include "Crypto.h"
#include "Curve25519.h"

// 2^252 + 27742317777372353535851937790883648493
static limb_t const numQ[NUM_LIMBS_256BIT] PROGMEM = {
    LIMB_PAIR(0x5CF5D3ED, 0x5812631A), LIMB_PAIR(0xA2F79CD6, 0x14DEF9DE),
    LIMB_PAIR(0x00000000, 0x00000000), LIMB_PAIR(0x00000000, 0x10000000)
};

static crypto_uint64 load_3(const unsigned char *in)
{
    crypto_uint64 result;
    result = (crypto_uint64)in[0];
    result |= ((crypto_uint64)in[1]) << 8;
    result |= ((crypto_uint64)in[2]) << 16;
    return result;
}

static crypto_uint64 load_4(const unsigned char *in)
{
    crypto_uint64 result;
    result = (crypto_uint64)in[0];
    result |= ((crypto_uint64)in[1]) << 8;
    result |= ((crypto_uint64)in[2]) << 16;
    result |= ((crypto_uint64)in[3]) << 24;
    return result;
}

void fe_sub(fe h, fe f, fe g)
{
    crypto_int32 f0 = f[0];
    crypto_int32 f1 = f[1];
    crypto_int32 f2 = f[2];
    crypto_int32 f3 = f[3];
    crypto_int32 f4 = f[4];
    crypto_int32 f5 = f[5];
    crypto_int32 f6 = f[6];
    crypto_int32 f7 = f[7];
    crypto_int32 f8 = f[8];
    crypto_int32 f9 = f[9];
    crypto_int32 g0 = g[0];
    crypto_int32 g1 = g[1];
    crypto_int32 g2 = g[2];
    crypto_int32 g3 = g[3];
    crypto_int32 g4 = g[4];
    crypto_int32 g5 = g[5];
    crypto_int32 g6 = g[6];
    crypto_int32 g7 = g[7];
    crypto_int32 g8 = g[8];
    crypto_int32 g9 = g[9];
    crypto_int32 h0 = f0 - g0;
    crypto_int32 h1 = f1 - g1;
    crypto_int32 h2 = f2 - g2;
    crypto_int32 h3 = f3 - g3;
    crypto_int32 h4 = f4 - g4;
    crypto_int32 h5 = f5 - g5;
    crypto_int32 h6 = f6 - g6;
    crypto_int32 h7 = f7 - g7;
    crypto_int32 h8 = f8 - g8;
    crypto_int32 h9 = f9 - g9;
    h[0] = h0;
    h[1] = h1;
    h[2] = h2;
    h[3] = h3;
    h[4] = h4;
    h[5] = h5;
    h[6] = h6;
    h[7] = h7;
    h[8] = h8;
    h[9] = h9;
}

void fe_frombytes(fe h, const unsigned char *s)
{
    crypto_int64 h0 = load_4(s);
    crypto_int64 h1 = load_3(s + 4) << 6;
    crypto_int64 h2 = load_3(s + 7) << 5;
    crypto_int64 h3 = load_3(s + 10) << 3;
    crypto_int64 h4 = load_3(s + 13) << 2;
    crypto_int64 h5 = load_4(s + 16);
    crypto_int64 h6 = load_3(s + 20) << 7;
    crypto_int64 h7 = load_3(s + 23) << 5;
    crypto_int64 h8 = load_3(s + 26) << 4;
    crypto_int64 h9 = (load_3(s + 29) & 8388607) << 2;
    crypto_int64 carry0;
    crypto_int64 carry1;
    crypto_int64 carry2;
    crypto_int64 carry3;
    crypto_int64 carry4;
    crypto_int64 carry5;
    crypto_int64 carry6;
    crypto_int64 carry7;
    crypto_int64 carry8;
    crypto_int64 carry9;

    carry9 = (h9 + (crypto_int64)(1 << 24)) >> 25;
    h0 += carry9 * 19;
    h9 -= carry9 << 25;
    carry1 = (h1 + (crypto_int64)(1 << 24)) >> 25;
    h2 += carry1;
    h1 -= carry1 << 25;
    carry3 = (h3 + (crypto_int64)(1 << 24)) >> 25;
    h4 += carry3;
    h3 -= carry3 << 25;
    carry5 = (h5 + (crypto_int64)(1 << 24)) >> 25;
    h6 += carry5;
    h5 -= carry5 << 25;
    carry7 = (h7 + (crypto_int64)(1 << 24)) >> 25;
    h8 += carry7;
    h7 -= carry7 << 25;

    carry0 = (h0 + (crypto_int64)(1 << 25)) >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;
    carry2 = (h2 + (crypto_int64)(1 << 25)) >> 26;
    h3 += carry2;
    h2 -= carry2 << 26;
    carry4 = (h4 + (crypto_int64)(1 << 25)) >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;
    carry6 = (h6 + (crypto_int64)(1 << 25)) >> 26;
    h7 += carry6;
    h6 -= carry6 << 26;
    carry8 = (h8 + (crypto_int64)(1 << 25)) >> 26;
    h9 += carry8;
    h8 -= carry8 << 26;

    h[0] = h0;
    h[1] = h1;
    h[2] = h2;
    h[3] = h3;
    h[4] = h4;
    h[5] = h5;
    h[6] = h6;
    h[7] = h7;
    h[8] = h8;
    h[9] = h9;
}

void fe_1(fe h)
{
    h[0] = 1;
    h[1] = 0;
    h[2] = 0;
    h[3] = 0;
    h[4] = 0;
    h[5] = 0;
    h[6] = 0;
    h[7] = 0;
    h[8] = 0;
    h[9] = 0;
}

void fe_add(fe h, const fe f, const fe g)
{
    crypto_int32 f0 = f[0];
    crypto_int32 f1 = f[1];
    crypto_int32 f2 = f[2];
    crypto_int32 f3 = f[3];
    crypto_int32 f4 = f[4];
    crypto_int32 f5 = f[5];
    crypto_int32 f6 = f[6];
    crypto_int32 f7 = f[7];
    crypto_int32 f8 = f[8];
    crypto_int32 f9 = f[9];
    crypto_int32 g0 = g[0];
    crypto_int32 g1 = g[1];
    crypto_int32 g2 = g[2];
    crypto_int32 g3 = g[3];
    crypto_int32 g4 = g[4];
    crypto_int32 g5 = g[5];
    crypto_int32 g6 = g[6];
    crypto_int32 g7 = g[7];
    crypto_int32 g8 = g[8];
    crypto_int32 g9 = g[9];
    crypto_int32 h0 = f0 + g0;
    crypto_int32 h1 = f1 + g1;
    crypto_int32 h2 = f2 + g2;
    crypto_int32 h3 = f3 + g3;
    crypto_int32 h4 = f4 + g4;
    crypto_int32 h5 = f5 + g5;
    crypto_int32 h6 = f6 + g6;
    crypto_int32 h7 = f7 + g7;
    crypto_int32 h8 = f8 + g8;
    crypto_int32 h9 = f9 + g9;
    h[0] = h0;
    h[1] = h1;
    h[2] = h2;
    h[3] = h3;
    h[4] = h4;
    h[5] = h5;
    h[6] = h6;
    h[7] = h7;
    h[8] = h8;
    h[9] = h9;
}

void fe_sq(fe h, const fe f)
{
    crypto_int32 f0 = f[0];
    crypto_int32 f1 = f[1];
    crypto_int32 f2 = f[2];
    crypto_int32 f3 = f[3];
    crypto_int32 f4 = f[4];
    crypto_int32 f5 = f[5];
    crypto_int32 f6 = f[6];
    crypto_int32 f7 = f[7];
    crypto_int32 f8 = f[8];
    crypto_int32 f9 = f[9];
    crypto_int32 f0_2 = 2 * f0;
    crypto_int32 f1_2 = 2 * f1;
    crypto_int32 f2_2 = 2 * f2;
    crypto_int32 f3_2 = 2 * f3;
    crypto_int32 f4_2 = 2 * f4;
    crypto_int32 f5_2 = 2 * f5;
    crypto_int32 f6_2 = 2 * f6;
    crypto_int32 f7_2 = 2 * f7;
    crypto_int32 f5_38 = 38 * f5; /* 1.959375*2^30 */
    crypto_int32 f6_19 = 19 * f6; /* 1.959375*2^30 */
    crypto_int32 f7_38 = 38 * f7; /* 1.959375*2^30 */
    crypto_int32 f8_19 = 19 * f8; /* 1.959375*2^30 */
    crypto_int32 f9_38 = 38 * f9; /* 1.959375*2^30 */
    crypto_int64 f0f0 = f0 * (crypto_int64)f0;
    crypto_int64 f0f1_2 = f0_2 * (crypto_int64)f1;
    crypto_int64 f0f2_2 = f0_2 * (crypto_int64)f2;
    crypto_int64 f0f3_2 = f0_2 * (crypto_int64)f3;
    crypto_int64 f0f4_2 = f0_2 * (crypto_int64)f4;
    crypto_int64 f0f5_2 = f0_2 * (crypto_int64)f5;
    crypto_int64 f0f6_2 = f0_2 * (crypto_int64)f6;
    crypto_int64 f0f7_2 = f0_2 * (crypto_int64)f7;
    crypto_int64 f0f8_2 = f0_2 * (crypto_int64)f8;
    crypto_int64 f0f9_2 = f0_2 * (crypto_int64)f9;
    crypto_int64 f1f1_2 = f1_2 * (crypto_int64)f1;
    crypto_int64 f1f2_2 = f1_2 * (crypto_int64)f2;
    crypto_int64 f1f3_4 = f1_2 * (crypto_int64)f3_2;
    crypto_int64 f1f4_2 = f1_2 * (crypto_int64)f4;
    crypto_int64 f1f5_4 = f1_2 * (crypto_int64)f5_2;
    crypto_int64 f1f6_2 = f1_2 * (crypto_int64)f6;
    crypto_int64 f1f7_4 = f1_2 * (crypto_int64)f7_2;
    crypto_int64 f1f8_2 = f1_2 * (crypto_int64)f8;
    crypto_int64 f1f9_76 = f1_2 * (crypto_int64)f9_38;
    crypto_int64 f2f2 = f2 * (crypto_int64)f2;
    crypto_int64 f2f3_2 = f2_2 * (crypto_int64)f3;
    crypto_int64 f2f4_2 = f2_2 * (crypto_int64)f4;
    crypto_int64 f2f5_2 = f2_2 * (crypto_int64)f5;
    crypto_int64 f2f6_2 = f2_2 * (crypto_int64)f6;
    crypto_int64 f2f7_2 = f2_2 * (crypto_int64)f7;
    crypto_int64 f2f8_38 = f2_2 * (crypto_int64)f8_19;
    crypto_int64 f2f9_38 = f2 * (crypto_int64)f9_38;
    crypto_int64 f3f3_2 = f3_2 * (crypto_int64)f3;
    crypto_int64 f3f4_2 = f3_2 * (crypto_int64)f4;
    crypto_int64 f3f5_4 = f3_2 * (crypto_int64)f5_2;
    crypto_int64 f3f6_2 = f3_2 * (crypto_int64)f6;
    crypto_int64 f3f7_76 = f3_2 * (crypto_int64)f7_38;
    crypto_int64 f3f8_38 = f3_2 * (crypto_int64)f8_19;
    crypto_int64 f3f9_76 = f3_2 * (crypto_int64)f9_38;
    crypto_int64 f4f4 = f4 * (crypto_int64)f4;
    crypto_int64 f4f5_2 = f4_2 * (crypto_int64)f5;
    crypto_int64 f4f6_38 = f4_2 * (crypto_int64)f6_19;
    crypto_int64 f4f7_38 = f4 * (crypto_int64)f7_38;
    crypto_int64 f4f8_38 = f4_2 * (crypto_int64)f8_19;
    crypto_int64 f4f9_38 = f4 * (crypto_int64)f9_38;
    crypto_int64 f5f5_38 = f5 * (crypto_int64)f5_38;
    crypto_int64 f5f6_38 = f5_2 * (crypto_int64)f6_19;
    crypto_int64 f5f7_76 = f5_2 * (crypto_int64)f7_38;
    crypto_int64 f5f8_38 = f5_2 * (crypto_int64)f8_19;
    crypto_int64 f5f9_76 = f5_2 * (crypto_int64)f9_38;
    crypto_int64 f6f6_19 = f6 * (crypto_int64)f6_19;
    crypto_int64 f6f7_38 = f6 * (crypto_int64)f7_38;
    crypto_int64 f6f8_38 = f6_2 * (crypto_int64)f8_19;
    crypto_int64 f6f9_38 = f6 * (crypto_int64)f9_38;
    crypto_int64 f7f7_38 = f7 * (crypto_int64)f7_38;
    crypto_int64 f7f8_38 = f7_2 * (crypto_int64)f8_19;
    crypto_int64 f7f9_76 = f7_2 * (crypto_int64)f9_38;
    crypto_int64 f8f8_19 = f8 * (crypto_int64)f8_19;
    crypto_int64 f8f9_38 = f8 * (crypto_int64)f9_38;
    crypto_int64 f9f9_38 = f9 * (crypto_int64)f9_38;
    crypto_int64 h0 = f0f0 + f1f9_76 + f2f8_38 + f3f7_76 + f4f6_38 + f5f5_38;
    crypto_int64 h1 = f0f1_2 + f2f9_38 + f3f8_38 + f4f7_38 + f5f6_38;
    crypto_int64 h2 = f0f2_2 + f1f1_2 + f3f9_76 + f4f8_38 + f5f7_76 + f6f6_19;
    crypto_int64 h3 = f0f3_2 + f1f2_2 + f4f9_38 + f5f8_38 + f6f7_38;
    crypto_int64 h4 = f0f4_2 + f1f3_4 + f2f2 + f5f9_76 + f6f8_38 + f7f7_38;
    crypto_int64 h5 = f0f5_2 + f1f4_2 + f2f3_2 + f6f9_38 + f7f8_38;
    crypto_int64 h6 = f0f6_2 + f1f5_4 + f2f4_2 + f3f3_2 + f7f9_76 + f8f8_19;
    crypto_int64 h7 = f0f7_2 + f1f6_2 + f2f5_2 + f3f4_2 + f8f9_38;
    crypto_int64 h8 = f0f8_2 + f1f7_4 + f2f6_2 + f3f5_4 + f4f4 + f9f9_38;
    crypto_int64 h9 = f0f9_2 + f1f8_2 + f2f7_2 + f3f6_2 + f4f5_2;
    crypto_int64 carry0;
    crypto_int64 carry1;
    crypto_int64 carry2;
    crypto_int64 carry3;
    crypto_int64 carry4;
    crypto_int64 carry5;
    crypto_int64 carry6;
    crypto_int64 carry7;
    crypto_int64 carry8;
    crypto_int64 carry9;

    carry0 = (h0 + (crypto_int64)(1 << 25)) >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;
    carry4 = (h4 + (crypto_int64)(1 << 25)) >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;

    carry1 = (h1 + (crypto_int64)(1 << 24)) >> 25;
    h2 += carry1;
    h1 -= carry1 << 25;
    carry5 = (h5 + (crypto_int64)(1 << 24)) >> 25;
    h6 += carry5;
    h5 -= carry5 << 25;

    carry2 = (h2 + (crypto_int64)(1 << 25)) >> 26;
    h3 += carry2;
    h2 -= carry2 << 26;
    carry6 = (h6 + (crypto_int64)(1 << 25)) >> 26;
    h7 += carry6;
    h6 -= carry6 << 26;

    carry3 = (h3 + (crypto_int64)(1 << 24)) >> 25;
    h4 += carry3;
    h3 -= carry3 << 25;
    carry7 = (h7 + (crypto_int64)(1 << 24)) >> 25;
    h8 += carry7;
    h7 -= carry7 << 25;

    carry4 = (h4 + (crypto_int64)(1 << 25)) >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;
    carry8 = (h8 + (crypto_int64)(1 << 25)) >> 26;
    h9 += carry8;
    h8 -= carry8 << 26;

    carry9 = (h9 + (crypto_int64)(1 << 24)) >> 25;
    h0 += carry9 * 19;
    h9 -= carry9 << 25;

    carry0 = (h0 + (crypto_int64)(1 << 25)) >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;

    h[0] = h0;
    h[1] = h1;
    h[2] = h2;
    h[3] = h3;
    h[4] = h4;
    h[5] = h5;
    h[6] = h6;
    h[7] = h7;
    h[8] = h8;
    h[9] = h9;
}

void fe_mul(fe h, const fe f, const fe g)
{
    crypto_int32 f0 = f[0];
    crypto_int32 f1 = f[1];
    crypto_int32 f2 = f[2];
    crypto_int32 f3 = f[3];
    crypto_int32 f4 = f[4];
    crypto_int32 f5 = f[5];
    crypto_int32 f6 = f[6];
    crypto_int32 f7 = f[7];
    crypto_int32 f8 = f[8];
    crypto_int32 f9 = f[9];
    crypto_int32 g0 = g[0];
    crypto_int32 g1 = g[1];
    crypto_int32 g2 = g[2];
    crypto_int32 g3 = g[3];
    crypto_int32 g4 = g[4];
    crypto_int32 g5 = g[5];
    crypto_int32 g6 = g[6];
    crypto_int32 g7 = g[7];
    crypto_int32 g8 = g[8];
    crypto_int32 g9 = g[9];
    crypto_int32 g1_19 = 19 * g1; /* 1.959375*2^29 */
    crypto_int32 g2_19 = 19 * g2; /* 1.959375*2^30; still ok */
    crypto_int32 g3_19 = 19 * g3;
    crypto_int32 g4_19 = 19 * g4;
    crypto_int32 g5_19 = 19 * g5;
    crypto_int32 g6_19 = 19 * g6;
    crypto_int32 g7_19 = 19 * g7;
    crypto_int32 g8_19 = 19 * g8;
    crypto_int32 g9_19 = 19 * g9;
    crypto_int32 f1_2 = 2 * f1;
    crypto_int32 f3_2 = 2 * f3;
    crypto_int32 f5_2 = 2 * f5;
    crypto_int32 f7_2 = 2 * f7;
    crypto_int32 f9_2 = 2 * f9;
    crypto_int64 f0g0 = f0 * (crypto_int64)g0;
    crypto_int64 f0g1 = f0 * (crypto_int64)g1;
    crypto_int64 f0g2 = f0 * (crypto_int64)g2;
    crypto_int64 f0g3 = f0 * (crypto_int64)g3;
    crypto_int64 f0g4 = f0 * (crypto_int64)g4;
    crypto_int64 f0g5 = f0 * (crypto_int64)g5;
    crypto_int64 f0g6 = f0 * (crypto_int64)g6;
    crypto_int64 f0g7 = f0 * (crypto_int64)g7;
    crypto_int64 f0g8 = f0 * (crypto_int64)g8;
    crypto_int64 f0g9 = f0 * (crypto_int64)g9;
    crypto_int64 f1g0 = f1 * (crypto_int64)g0;
    crypto_int64 f1g1_2 = f1_2 * (crypto_int64)g1;
    crypto_int64 f1g2 = f1 * (crypto_int64)g2;
    crypto_int64 f1g3_2 = f1_2 * (crypto_int64)g3;
    crypto_int64 f1g4 = f1 * (crypto_int64)g4;
    crypto_int64 f1g5_2 = f1_2 * (crypto_int64)g5;
    crypto_int64 f1g6 = f1 * (crypto_int64)g6;
    crypto_int64 f1g7_2 = f1_2 * (crypto_int64)g7;
    crypto_int64 f1g8 = f1 * (crypto_int64)g8;
    crypto_int64 f1g9_38 = f1_2 * (crypto_int64)g9_19;
    crypto_int64 f2g0 = f2 * (crypto_int64)g0;
    crypto_int64 f2g1 = f2 * (crypto_int64)g1;
    crypto_int64 f2g2 = f2 * (crypto_int64)g2;
    crypto_int64 f2g3 = f2 * (crypto_int64)g3;
    crypto_int64 f2g4 = f2 * (crypto_int64)g4;
    crypto_int64 f2g5 = f2 * (crypto_int64)g5;
    crypto_int64 f2g6 = f2 * (crypto_int64)g6;
    crypto_int64 f2g7 = f2 * (crypto_int64)g7;
    crypto_int64 f2g8_19 = f2 * (crypto_int64)g8_19;
    crypto_int64 f2g9_19 = f2 * (crypto_int64)g9_19;
    crypto_int64 f3g0 = f3 * (crypto_int64)g0;
    crypto_int64 f3g1_2 = f3_2 * (crypto_int64)g1;
    crypto_int64 f3g2 = f3 * (crypto_int64)g2;
    crypto_int64 f3g3_2 = f3_2 * (crypto_int64)g3;
    crypto_int64 f3g4 = f3 * (crypto_int64)g4;
    crypto_int64 f3g5_2 = f3_2 * (crypto_int64)g5;
    crypto_int64 f3g6 = f3 * (crypto_int64)g6;
    crypto_int64 f3g7_38 = f3_2 * (crypto_int64)g7_19;
    crypto_int64 f3g8_19 = f3 * (crypto_int64)g8_19;
    crypto_int64 f3g9_38 = f3_2 * (crypto_int64)g9_19;
    crypto_int64 f4g0 = f4 * (crypto_int64)g0;
    crypto_int64 f4g1 = f4 * (crypto_int64)g1;
    crypto_int64 f4g2 = f4 * (crypto_int64)g2;
    crypto_int64 f4g3 = f4 * (crypto_int64)g3;
    crypto_int64 f4g4 = f4 * (crypto_int64)g4;
    crypto_int64 f4g5 = f4 * (crypto_int64)g5;
    crypto_int64 f4g6_19 = f4 * (crypto_int64)g6_19;
    crypto_int64 f4g7_19 = f4 * (crypto_int64)g7_19;
    crypto_int64 f4g8_19 = f4 * (crypto_int64)g8_19;
    crypto_int64 f4g9_19 = f4 * (crypto_int64)g9_19;
    crypto_int64 f5g0 = f5 * (crypto_int64)g0;
    crypto_int64 f5g1_2 = f5_2 * (crypto_int64)g1;
    crypto_int64 f5g2 = f5 * (crypto_int64)g2;
    crypto_int64 f5g3_2 = f5_2 * (crypto_int64)g3;
    crypto_int64 f5g4 = f5 * (crypto_int64)g4;
    crypto_int64 f5g5_38 = f5_2 * (crypto_int64)g5_19;
    crypto_int64 f5g6_19 = f5 * (crypto_int64)g6_19;
    crypto_int64 f5g7_38 = f5_2 * (crypto_int64)g7_19;
    crypto_int64 f5g8_19 = f5 * (crypto_int64)g8_19;
    crypto_int64 f5g9_38 = f5_2 * (crypto_int64)g9_19;
    crypto_int64 f6g0 = f6 * (crypto_int64)g0;
    crypto_int64 f6g1 = f6 * (crypto_int64)g1;
    crypto_int64 f6g2 = f6 * (crypto_int64)g2;
    crypto_int64 f6g3 = f6 * (crypto_int64)g3;
    crypto_int64 f6g4_19 = f6 * (crypto_int64)g4_19;
    crypto_int64 f6g5_19 = f6 * (crypto_int64)g5_19;
    crypto_int64 f6g6_19 = f6 * (crypto_int64)g6_19;
    crypto_int64 f6g7_19 = f6 * (crypto_int64)g7_19;
    crypto_int64 f6g8_19 = f6 * (crypto_int64)g8_19;
    crypto_int64 f6g9_19 = f6 * (crypto_int64)g9_19;
    crypto_int64 f7g0 = f7 * (crypto_int64)g0;
    crypto_int64 f7g1_2 = f7_2 * (crypto_int64)g1;
    crypto_int64 f7g2 = f7 * (crypto_int64)g2;
    crypto_int64 f7g3_38 = f7_2 * (crypto_int64)g3_19;
    crypto_int64 f7g4_19 = f7 * (crypto_int64)g4_19;
    crypto_int64 f7g5_38 = f7_2 * (crypto_int64)g5_19;
    crypto_int64 f7g6_19 = f7 * (crypto_int64)g6_19;
    crypto_int64 f7g7_38 = f7_2 * (crypto_int64)g7_19;
    crypto_int64 f7g8_19 = f7 * (crypto_int64)g8_19;
    crypto_int64 f7g9_38 = f7_2 * (crypto_int64)g9_19;
    crypto_int64 f8g0 = f8 * (crypto_int64)g0;
    crypto_int64 f8g1 = f8 * (crypto_int64)g1;
    crypto_int64 f8g2_19 = f8 * (crypto_int64)g2_19;
    crypto_int64 f8g3_19 = f8 * (crypto_int64)g3_19;
    crypto_int64 f8g4_19 = f8 * (crypto_int64)g4_19;
    crypto_int64 f8g5_19 = f8 * (crypto_int64)g5_19;
    crypto_int64 f8g6_19 = f8 * (crypto_int64)g6_19;
    crypto_int64 f8g7_19 = f8 * (crypto_int64)g7_19;
    crypto_int64 f8g8_19 = f8 * (crypto_int64)g8_19;
    crypto_int64 f8g9_19 = f8 * (crypto_int64)g9_19;
    crypto_int64 f9g0 = f9 * (crypto_int64)g0;
    crypto_int64 f9g1_38 = f9_2 * (crypto_int64)g1_19;
    crypto_int64 f9g2_19 = f9 * (crypto_int64)g2_19;
    crypto_int64 f9g3_38 = f9_2 * (crypto_int64)g3_19;
    crypto_int64 f9g4_19 = f9 * (crypto_int64)g4_19;
    crypto_int64 f9g5_38 = f9_2 * (crypto_int64)g5_19;
    crypto_int64 f9g6_19 = f9 * (crypto_int64)g6_19;
    crypto_int64 f9g7_38 = f9_2 * (crypto_int64)g7_19;
    crypto_int64 f9g8_19 = f9 * (crypto_int64)g8_19;
    crypto_int64 f9g9_38 = f9_2 * (crypto_int64)g9_19;
    crypto_int64 h0 = f0g0 + f1g9_38 + f2g8_19 + f3g7_38 + f4g6_19 + f5g5_38 + f6g4_19 + f7g3_38 + f8g2_19 + f9g1_38;
    crypto_int64 h1 = f0g1 + f1g0 + f2g9_19 + f3g8_19 + f4g7_19 + f5g6_19 + f6g5_19 + f7g4_19 + f8g3_19 + f9g2_19;
    crypto_int64 h2 = f0g2 + f1g1_2 + f2g0 + f3g9_38 + f4g8_19 + f5g7_38 + f6g6_19 + f7g5_38 + f8g4_19 + f9g3_38;
    crypto_int64 h3 = f0g3 + f1g2 + f2g1 + f3g0 + f4g9_19 + f5g8_19 + f6g7_19 + f7g6_19 + f8g5_19 + f9g4_19;
    crypto_int64 h4 = f0g4 + f1g3_2 + f2g2 + f3g1_2 + f4g0 + f5g9_38 + f6g8_19 + f7g7_38 + f8g6_19 + f9g5_38;
    crypto_int64 h5 = f0g5 + f1g4 + f2g3 + f3g2 + f4g1 + f5g0 + f6g9_19 + f7g8_19 + f8g7_19 + f9g6_19;
    crypto_int64 h6 = f0g6 + f1g5_2 + f2g4 + f3g3_2 + f4g2 + f5g1_2 + f6g0 + f7g9_38 + f8g8_19 + f9g7_38;
    crypto_int64 h7 = f0g7 + f1g6 + f2g5 + f3g4 + f4g3 + f5g2 + f6g1 + f7g0 + f8g9_19 + f9g8_19;
    crypto_int64 h8 = f0g8 + f1g7_2 + f2g6 + f3g5_2 + f4g4 + f5g3_2 + f6g2 + f7g1_2 + f8g0 + f9g9_38;
    crypto_int64 h9 = f0g9 + f1g8 + f2g7 + f3g6 + f4g5 + f5g4 + f6g3 + f7g2 + f8g1 + f9g0;
    crypto_int64 carry0;
    crypto_int64 carry1;
    crypto_int64 carry2;
    crypto_int64 carry3;
    crypto_int64 carry4;
    crypto_int64 carry5;
    crypto_int64 carry6;
    crypto_int64 carry7;
    crypto_int64 carry8;
    crypto_int64 carry9;

    /*
    |h0| <= (1.65*1.65*2^52*(1+19+19+19+19)+1.65*1.65*2^50*(38+38+38+38+38))
      i.e. |h0| <= 1.4*2^60; narrower ranges for h2, h4, h6, h8
    |h1| <= (1.65*1.65*2^51*(1+1+19+19+19+19+19+19+19+19))
      i.e. |h1| <= 1.7*2^59; narrower ranges for h3, h5, h7, h9
    */

    carry0 = (h0 + (crypto_int64)(1 << 25)) >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;
    carry4 = (h4 + (crypto_int64)(1 << 25)) >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;
    /* |h0| <= 2^25 */
    /* |h4| <= 2^25 */
    /* |h1| <= 1.71*2^59 */
    /* |h5| <= 1.71*2^59 */

    carry1 = (h1 + (crypto_int64)(1 << 24)) >> 25;
    h2 += carry1;
    h1 -= carry1 << 25;
    carry5 = (h5 + (crypto_int64)(1 << 24)) >> 25;
    h6 += carry5;
    h5 -= carry5 << 25;
    /* |h1| <= 2^24; from now on fits into int32 */
    /* |h5| <= 2^24; from now on fits into int32 */
    /* |h2| <= 1.41*2^60 */
    /* |h6| <= 1.41*2^60 */

    carry2 = (h2 + (crypto_int64)(1 << 25)) >> 26;
    h3 += carry2;
    h2 -= carry2 << 26;
    carry6 = (h6 + (crypto_int64)(1 << 25)) >> 26;
    h7 += carry6;
    h6 -= carry6 << 26;
    /* |h2| <= 2^25; from now on fits into int32 unchanged */
    /* |h6| <= 2^25; from now on fits into int32 unchanged */
    /* |h3| <= 1.71*2^59 */
    /* |h7| <= 1.71*2^59 */

    carry3 = (h3 + (crypto_int64)(1 << 24)) >> 25;
    h4 += carry3;
    h3 -= carry3 << 25;
    carry7 = (h7 + (crypto_int64)(1 << 24)) >> 25;
    h8 += carry7;
    h7 -= carry7 << 25;
    /* |h3| <= 2^24; from now on fits into int32 unchanged */
    /* |h7| <= 2^24; from now on fits into int32 unchanged */
    /* |h4| <= 1.72*2^34 */
    /* |h8| <= 1.41*2^60 */

    carry4 = (h4 + (crypto_int64)(1 << 25)) >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;
    carry8 = (h8 + (crypto_int64)(1 << 25)) >> 26;
    h9 += carry8;
    h8 -= carry8 << 26;
    /* |h4| <= 2^25; from now on fits into int32 unchanged */
    /* |h8| <= 2^25; from now on fits into int32 unchanged */
    /* |h5| <= 1.01*2^24 */
    /* |h9| <= 1.71*2^59 */

    carry9 = (h9 + (crypto_int64)(1 << 24)) >> 25;
    h0 += carry9 * 19;
    h9 -= carry9 << 25;
    /* |h9| <= 2^24; from now on fits into int32 unchanged */
    /* |h0| <= 1.1*2^39 */

    carry0 = (h0 + (crypto_int64)(1 << 25)) >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;
    /* |h0| <= 2^25; from now on fits into int32 unchanged */
    /* |h1| <= 1.01*2^24 */

    h[0] = h0;
    h[1] = h1;
    h[2] = h2;
    h[3] = h3;
    h[4] = h4;
    h[5] = h5;
    h[6] = h6;
    h[7] = h7;
    h[8] = h8;
    h[9] = h9;
}

void fe_invert(fe out, const fe z)
{
    fe t0;
    fe t1;
    fe t2;
    fe t3;
    int i;

    /* qhasm: z2 = z1^2^1 */
    /* asm 1: fe_sq(>z2=fe#1,<z1=fe#11); for (i = 1;i < 1;++i) fe_sq(>z2=fe#1,>z2=fe#1); */
    /* asm 2: fe_sq(>z2=t0,<z1=z); for (i = 1;i < 1;++i) fe_sq(>z2=t0,>z2=t0); */
    fe_sq(t0, z);
    for (i = 1; i < 1; ++i)
        fe_sq(t0, t0);

    /* qhasm: z8 = z2^2^2 */
    /* asm 1: fe_sq(>z8=fe#2,<z2=fe#1); for (i = 1;i < 2;++i) fe_sq(>z8=fe#2,>z8=fe#2); */
    /* asm 2: fe_sq(>z8=t1,<z2=t0); for (i = 1;i < 2;++i) fe_sq(>z8=t1,>z8=t1); */
    fe_sq(t1, t0);
    for (i = 1; i < 2; ++i)
        fe_sq(t1, t1);

    /* qhasm: z9 = z1*z8 */
    /* asm 1: fe_mul(>z9=fe#2,<z1=fe#11,<z8=fe#2); */
    /* asm 2: fe_mul(>z9=t1,<z1=z,<z8=t1); */
    fe_mul(t1, z, t1);

    /* qhasm: z11 = z2*z9 */
    /* asm 1: fe_mul(>z11=fe#1,<z2=fe#1,<z9=fe#2); */
    /* asm 2: fe_mul(>z11=t0,<z2=t0,<z9=t1); */
    fe_mul(t0, t0, t1);

    /* qhasm: z22 = z11^2^1 */
    /* asm 1: fe_sq(>z22=fe#3,<z11=fe#1); for (i = 1;i < 1;++i) fe_sq(>z22=fe#3,>z22=fe#3); */
    /* asm 2: fe_sq(>z22=t2,<z11=t0); for (i = 1;i < 1;++i) fe_sq(>z22=t2,>z22=t2); */
    fe_sq(t2, t0);
    for (i = 1; i < 1; ++i)
        fe_sq(t2, t2);

    /* qhasm: z_5_0 = z9*z22 */
    /* asm 1: fe_mul(>z_5_0=fe#2,<z9=fe#2,<z22=fe#3); */
    /* asm 2: fe_mul(>z_5_0=t1,<z9=t1,<z22=t2); */
    fe_mul(t1, t1, t2);

    /* qhasm: z_10_5 = z_5_0^2^5 */
    /* asm 1: fe_sq(>z_10_5=fe#3,<z_5_0=fe#2); for (i = 1;i < 5;++i) fe_sq(>z_10_5=fe#3,>z_10_5=fe#3); */
    /* asm 2: fe_sq(>z_10_5=t2,<z_5_0=t1); for (i = 1;i < 5;++i) fe_sq(>z_10_5=t2,>z_10_5=t2); */
    fe_sq(t2, t1);
    for (i = 1; i < 5; ++i)
        fe_sq(t2, t2);

    /* qhasm: z_10_0 = z_10_5*z_5_0 */
    /* asm 1: fe_mul(>z_10_0=fe#2,<z_10_5=fe#3,<z_5_0=fe#2); */
    /* asm 2: fe_mul(>z_10_0=t1,<z_10_5=t2,<z_5_0=t1); */
    fe_mul(t1, t2, t1);

    /* qhasm: z_20_10 = z_10_0^2^10 */
    /* asm 1: fe_sq(>z_20_10=fe#3,<z_10_0=fe#2); for (i = 1;i < 10;++i) fe_sq(>z_20_10=fe#3,>z_20_10=fe#3); */
    /* asm 2: fe_sq(>z_20_10=t2,<z_10_0=t1); for (i = 1;i < 10;++i) fe_sq(>z_20_10=t2,>z_20_10=t2); */
    fe_sq(t2, t1);
    for (i = 1; i < 10; ++i)
        fe_sq(t2, t2);

    /* qhasm: z_20_0 = z_20_10*z_10_0 */
    /* asm 1: fe_mul(>z_20_0=fe#3,<z_20_10=fe#3,<z_10_0=fe#2); */
    /* asm 2: fe_mul(>z_20_0=t2,<z_20_10=t2,<z_10_0=t1); */
    fe_mul(t2, t2, t1);

    /* qhasm: z_40_20 = z_20_0^2^20 */
    /* asm 1: fe_sq(>z_40_20=fe#4,<z_20_0=fe#3); for (i = 1;i < 20;++i) fe_sq(>z_40_20=fe#4,>z_40_20=fe#4); */
    /* asm 2: fe_sq(>z_40_20=t3,<z_20_0=t2); for (i = 1;i < 20;++i) fe_sq(>z_40_20=t3,>z_40_20=t3); */
    fe_sq(t3, t2);
    for (i = 1; i < 20; ++i)
        fe_sq(t3, t3);

    /* qhasm: z_40_0 = z_40_20*z_20_0 */
    /* asm 1: fe_mul(>z_40_0=fe#3,<z_40_20=fe#4,<z_20_0=fe#3); */
    /* asm 2: fe_mul(>z_40_0=t2,<z_40_20=t3,<z_20_0=t2); */
    fe_mul(t2, t3, t2);

    /* qhasm: z_50_10 = z_40_0^2^10 */
    /* asm 1: fe_sq(>z_50_10=fe#3,<z_40_0=fe#3); for (i = 1;i < 10;++i) fe_sq(>z_50_10=fe#3,>z_50_10=fe#3); */
    /* asm 2: fe_sq(>z_50_10=t2,<z_40_0=t2); for (i = 1;i < 10;++i) fe_sq(>z_50_10=t2,>z_50_10=t2); */
    fe_sq(t2, t2);
    for (i = 1; i < 10; ++i)
        fe_sq(t2, t2);

    /* qhasm: z_50_0 = z_50_10*z_10_0 */
    /* asm 1: fe_mul(>z_50_0=fe#2,<z_50_10=fe#3,<z_10_0=fe#2); */
    /* asm 2: fe_mul(>z_50_0=t1,<z_50_10=t2,<z_10_0=t1); */
    fe_mul(t1, t2, t1);

    /* qhasm: z_100_50 = z_50_0^2^50 */
    /* asm 1: fe_sq(>z_100_50=fe#3,<z_50_0=fe#2); for (i = 1;i < 50;++i) fe_sq(>z_100_50=fe#3,>z_100_50=fe#3); */
    /* asm 2: fe_sq(>z_100_50=t2,<z_50_0=t1); for (i = 1;i < 50;++i) fe_sq(>z_100_50=t2,>z_100_50=t2); */
    fe_sq(t2, t1);
    for (i = 1; i < 50; ++i)
        fe_sq(t2, t2);

    /* qhasm: z_100_0 = z_100_50*z_50_0 */
    /* asm 1: fe_mul(>z_100_0=fe#3,<z_100_50=fe#3,<z_50_0=fe#2); */
    /* asm 2: fe_mul(>z_100_0=t2,<z_100_50=t2,<z_50_0=t1); */
    fe_mul(t2, t2, t1);

    /* qhasm: z_200_100 = z_100_0^2^100 */
    /* asm 1: fe_sq(>z_200_100=fe#4,<z_100_0=fe#3); for (i = 1;i < 100;++i) fe_sq(>z_200_100=fe#4,>z_200_100=fe#4); */
    /* asm 2: fe_sq(>z_200_100=t3,<z_100_0=t2); for (i = 1;i < 100;++i) fe_sq(>z_200_100=t3,>z_200_100=t3); */
    fe_sq(t3, t2);
    for (i = 1; i < 100; ++i)
        fe_sq(t3, t3);

    /* qhasm: z_200_0 = z_200_100*z_100_0 */
    /* asm 1: fe_mul(>z_200_0=fe#3,<z_200_100=fe#4,<z_100_0=fe#3); */
    /* asm 2: fe_mul(>z_200_0=t2,<z_200_100=t3,<z_100_0=t2); */
    fe_mul(t2, t3, t2);

    /* qhasm: z_250_50 = z_200_0^2^50 */
    /* asm 1: fe_sq(>z_250_50=fe#3,<z_200_0=fe#3); for (i = 1;i < 50;++i) fe_sq(>z_250_50=fe#3,>z_250_50=fe#3); */
    /* asm 2: fe_sq(>z_250_50=t2,<z_200_0=t2); for (i = 1;i < 50;++i) fe_sq(>z_250_50=t2,>z_250_50=t2); */
    fe_sq(t2, t2);
    for (i = 1; i < 50; ++i)
        fe_sq(t2, t2);

    /* qhasm: z_250_0 = z_250_50*z_50_0 */
    /* asm 1: fe_mul(>z_250_0=fe#2,<z_250_50=fe#3,<z_50_0=fe#2); */
    /* asm 2: fe_mul(>z_250_0=t1,<z_250_50=t2,<z_50_0=t1); */
    fe_mul(t1, t2, t1);

    /* qhasm: z_255_5 = z_250_0^2^5 */
    /* asm 1: fe_sq(>z_255_5=fe#2,<z_250_0=fe#2); for (i = 1;i < 5;++i) fe_sq(>z_255_5=fe#2,>z_255_5=fe#2); */
    /* asm 2: fe_sq(>z_255_5=t1,<z_250_0=t1); for (i = 1;i < 5;++i) fe_sq(>z_255_5=t1,>z_255_5=t1); */
    fe_sq(t1, t1);
    for (i = 1; i < 5; ++i)
        fe_sq(t1, t1);

    /* qhasm: z_255_21 = z_255_5*z11 */
    /* asm 1: fe_mul(>z_255_21=fe#12,<z_255_5=fe#2,<z11=fe#1); */
    /* asm 2: fe_mul(>z_255_21=out,<z_255_5=t1,<z11=t0); */
    fe_mul(out, t1, t0);

    /* qhasm: return */

    return;
}

void fe_tobytes(unsigned char *s, const fe h)
{
    crypto_int32 h0 = h[0];
    crypto_int32 h1 = h[1];
    crypto_int32 h2 = h[2];
    crypto_int32 h3 = h[3];
    crypto_int32 h4 = h[4];
    crypto_int32 h5 = h[5];
    crypto_int32 h6 = h[6];
    crypto_int32 h7 = h[7];
    crypto_int32 h8 = h[8];
    crypto_int32 h9 = h[9];
    crypto_int32 q;
    crypto_int32 carry0;
    crypto_int32 carry1;
    crypto_int32 carry2;
    crypto_int32 carry3;
    crypto_int32 carry4;
    crypto_int32 carry5;
    crypto_int32 carry6;
    crypto_int32 carry7;
    crypto_int32 carry8;
    crypto_int32 carry9;

    q = (19 * h9 + (((crypto_int32)1) << 24)) >> 25;
    q = (h0 + q) >> 26;
    q = (h1 + q) >> 25;
    q = (h2 + q) >> 26;
    q = (h3 + q) >> 25;
    q = (h4 + q) >> 26;
    q = (h5 + q) >> 25;
    q = (h6 + q) >> 26;
    q = (h7 + q) >> 25;
    q = (h8 + q) >> 26;
    q = (h9 + q) >> 25;

    /* Goal: Output h-(2^255-19)q, which is between 0 and 2^255-20. */
    h0 += 19 * q;
    /* Goal: Output h-2^255 q, which is between 0 and 2^255-20. */

    carry0 = h0 >> 26;
    h1 += carry0;
    h0 -= carry0 << 26;
    carry1 = h1 >> 25;
    h2 += carry1;
    h1 -= carry1 << 25;
    carry2 = h2 >> 26;
    h3 += carry2;
    h2 -= carry2 << 26;
    carry3 = h3 >> 25;
    h4 += carry3;
    h3 -= carry3 << 25;
    carry4 = h4 >> 26;
    h5 += carry4;
    h4 -= carry4 << 26;
    carry5 = h5 >> 25;
    h6 += carry5;
    h5 -= carry5 << 25;
    carry6 = h6 >> 26;
    h7 += carry6;
    h6 -= carry6 << 26;
    carry7 = h7 >> 25;
    h8 += carry7;
    h7 -= carry7 << 25;
    carry8 = h8 >> 26;
    h9 += carry8;
    h8 -= carry8 << 26;
    carry9 = h9 >> 25;
    h9 -= carry9 << 25;
    /* h10 = carry9 */

    /*
    Goal: Output h0+...+2^255 h10-2^255 q, which is between 0 and 2^255-20.
    Have h0+...+2^230 h9 between 0 and 2^255-1;
    evidently 2^255 h10-2^255 q = 0.
    Goal: Output h0+...+2^230 h9.
    */

    s[0] = h0 >> 0;
    s[1] = h0 >> 8;
    s[2] = h0 >> 16;
    s[3] = (h0 >> 24) | (h1 << 2);
    s[4] = h1 >> 6;
    s[5] = h1 >> 14;
    s[6] = (h1 >> 22) | (h2 << 3);
    s[7] = h2 >> 5;
    s[8] = h2 >> 13;
    s[9] = (h2 >> 21) | (h3 << 5);
    s[10] = h3 >> 3;
    s[11] = h3 >> 11;
    s[12] = (h3 >> 19) | (h4 << 6);
    s[13] = h4 >> 2;
    s[14] = h4 >> 10;
    s[15] = h4 >> 18;
    s[16] = h5 >> 0;
    s[17] = h5 >> 8;
    s[18] = h5 >> 16;
    s[19] = (h5 >> 24) | (h6 << 1);
    s[20] = h6 >> 7;
    s[21] = h6 >> 15;
    s[22] = (h6 >> 23) | (h7 << 3);
    s[23] = h7 >> 5;
    s[24] = h7 >> 13;
    s[25] = (h7 >> 21) | (h8 << 4);
    s[26] = h8 >> 4;
    s[27] = h8 >> 12;
    s[28] = (h8 >> 20) | (h9 << 6);
    s[29] = h9 >> 2;
    s[30] = h9 >> 10;
    s[31] = h9 >> 18;
}

void XEdDSA::priv_curve_to_ed_keys(uint8_t *curve_privkey, uint8_t *ed_privkey, uint8_t *ed_pubkey)
{
    limb_t a[NUM_LIMBS_256BIT];
    limb_t a2[NUM_LIMBS_256BIT];
    limb_t t[NUM_LIMBS_512BIT + 1];
    Ed25519::Point ptA;
    Ed25519::Point ptA2;

    for (uint8_t i = 0; i < 32; i++) {
        ed_privkey[i] = curve_privkey[i];
    }

    ed_privkey[0] &= 0xF8;
    ed_privkey[31] &= 0x7F;
    ed_privkey[31] |= 0x40;
    BigNumberUtil::unpackLE(a, NUM_LIMBS_256BIT, ed_privkey, 32);
    Ed25519::mul(ptA, a);
    Ed25519::encodePoint(ed_pubkey, ptA);

    clean(a);
    clean(ptA);

    // check sign
    if ((ed_pubkey[31] & 0x80) >> 7 == 0) {
        return;
    }

    // Negate the scalar mod q. The clamped key is below 2^255, so reduce it first.
    BigNumberUtil::unpackLE(t, NUM_LIMBS_512BIT, ed_privkey, 32);
    t[NUM_LIMBS_512BIT] = 0;
    reduceQ(a2, t);
    clean(t);
    BigNumberUtil::sub(a2, t, a2, NUM_LIMBS_256BIT);
    BigNumberUtil::add_P(a2, a2, numQ, NUM_LIMBS_256BIT);
    BigNumberUtil::reduceQuick_P(a2, a2, numQ, NUM_LIMBS_256BIT);
    BigNumberUtil::packLE(ed_privkey, 32, a2, NUM_LIMBS_256BIT);

    Ed25519::mul(ptA2, a2);
    Ed25519::encodePoint(ed_pubkey, ptA2);

    // Clean up and exit.
    clean(a2);
    clean(ptA2);
}

/**
 * \brief Derive key material from a 32-byte private key.
 *
 * \param hash SHA512 hash object from the caller for use in this function.
 * The 64-byte output buffer within this hash object will contain the
 * hash prefix on exit.
 * \param a The secret scalar derived from \a privateKey.  This must be
 * NUM_LIMBS_256BIT limbs in size.
 * \param privateKey The 32-byte private key to derive all other values from.
 */
void XEdDSA::deriveKeys(SHA512 *hash, limb_t *a, const uint8_t privateKey[32])
{
    uint8_t *buf = (uint8_t *)(hash->state.w); // Reuse hash buffer to save memory.

    // Very important, we hash the private key to get a secret for the nonce hash
    hash->reset();
    hash->update(privateKey, 32);
    hash->finalize(buf, 0);

    // Unpack the first half of the hash value into "a".
    BigNumberUtil::unpackLE(a, NUM_LIMBS_256BIT, privateKey, 32);
}

/**
 * \brief Signs a message using a specific Ed25519 private key.
 *
 * \param signature The signature value. Must contain at least 32 bytes of randomness when this function is called,
 *  and will be overwritten with the signature on exit.
 * \param privateKey The private key to use to sign the message.
 * \param publicKey The public key corresponding to \a privateKey.
 * \param message Points to the message to be signed.
 * \param len The length of the \a message to be signed.
 * \param baseMul Optional function that computes R = r * B, for example on a crypto
 *  accelerator. If it is null or returns false, R is computed in software. The
 *  signature is the same either way.
 * \param baseMulCtx Passed to \a baseMul unchanged.
 *
 * \sa verify(), derivePublicKey()
 */
void XEdDSA::sign(uint8_t signature[64], const uint8_t privateKey[32],
                   const uint8_t publicKey[32], const void *message, size_t len,
                   XEdDSABaseMulFn baseMul, void *baseMulCtx)
{
    SHA512 hash;
    uint8_t *buf = (uint8_t *)(hash.state.w); // Reuse hash buffer to save memory.
    limb_t a[NUM_LIMBS_256BIT];
    limb_t r[NUM_LIMBS_256BIT];
    limb_t k[NUM_LIMBS_256BIT];
    limb_t t[NUM_LIMBS_512BIT + 1];
    Point rB;

    // Derive the secret scalar a and the message prefix from the private key.
    deriveKeys(&hash, a, privateKey);

    // Hash the prefix and the message to derive r.
    hash.reset();
    hash.update(buf + 32, 32);
    hash.update(message, len);
    hash.update(signature, 32);
    hash.finalize(buf, 0);
    reduceQFromBuffer(r, buf, t);

    // Encode rB into the first half of the signature buffer as R.
    bool haveR = false;
    if (baseMul) {
        uint8_t rBytes[32];
        BigNumberUtil::packLE(rBytes, 32, r, NUM_LIMBS_256BIT);
        haveR = baseMul(baseMulCtx, signature, rBytes);
        clean(rBytes);
    }
    if (!haveR) {
        mul(rB, r);
        encodePoint(signature, rB);
    }

    // Hash R, A, and the message to get k.
    hash.reset();
    hash.update(signature, 32); // R
    hash.update(publicKey, 32); // A
    hash.update(message, len);
    hash.finalize(buf, 0);
    reduceQFromBuffer(k, buf, t);

    // Compute s = (r + k * a) mod q.
    Curve25519::mulNoReduce(t, k, a);
    t[NUM_LIMBS_512BIT] = 0;
    reduceQ(t, t);
    BigNumberUtil::add(t, t, r, NUM_LIMBS_256BIT);
    BigNumberUtil::reduceQuick_P(t, t, numQ, NUM_LIMBS_256BIT);
    BigNumberUtil::packLE(signature + 32, 32, t, NUM_LIMBS_256BIT);

    // Clean up.
    clean(a);
    clean(r);
    clean(k);
    clean(t);
    clean(rB);
}
