/******************************************************************************
*  Filename:       sw_poly1305-donna.c
*  Revised:        2016-10-05 12:42:03 +0200 (Wed, 05 Oct 2016)
*  Revision:       47308
******************************************************************************/

#include "sw_poly1305-donna.h"

#include "sw_poly1305-donna-32.h"

void
poly1305_update(poly1305_context *ctx, const unsigned char *m, size_t bytes) {
   poly1305_state_internal_t *st = (poly1305_state_internal_t *)ctx;
   size_t i;

   /* handle leftover */
   if (st->leftover) {
      size_t want = (poly1305_block_size - st->leftover);
      if (want > bytes)
         want = bytes;
      for (i = 0; i < want; i++)
         st->buffer[st->leftover + i] = m[i];
      bytes -= want;
      m += want;
      st->leftover += want;
      if (st->leftover < poly1305_block_size)
         return;
      poly1305_blocks(st, st->buffer, poly1305_block_size);
      st->leftover = 0;
   }

   /* process full blocks */
   if (bytes >= poly1305_block_size) {
      size_t want = (bytes & ~(poly1305_block_size - 1));
      poly1305_blocks(st, m, want);
      m += want;
      bytes -= want;
   }

   /* store leftover */
   if (bytes) {
      for (i = 0; i < bytes; i++)
         st->buffer[st->leftover + i] = m[i];
      st->leftover += bytes;
   }
}

void
poly1305_auth(unsigned char mac[16], const unsigned char *m, size_t bytes, const unsigned char key[32]) {
   poly1305_context ctx;
   poly1305_init(&ctx, key);
   poly1305_update(&ctx, m, bytes);
   poly1305_finish(&ctx, mac);
}

int
poly1305_verify(const unsigned char mac1[16], const unsigned char mac2[16]) {
   size_t i;
   unsigned int dif = 0;
   for (i = 0; i < 16; i++)
      dif |= (mac1[i] ^ mac2[i]);
   dif = (dif - 1) >> ((sizeof(unsigned int) * 8) - 1);
   return (dif & 1);
}


/* test a few basic operations */
int
poly1305_power_on_self_test(void) {
   /* example from nacl */
   static const unsigned char nacl_key[32] = {
      0xee,0xa6,0xa7,0x25,0x1c,0x1e,0x72,0x91,
      0x6d,0x11,0xc2,0xcb,0x21,0x4d,0x3c,0x25,
      0x25,0x39,0x12,0x1d,0x8e,0x23,0x4e,0x65,
      0x2d,0x65,0x1f,0xa4,0xc8,0xcf,0xf8,0x80
   };

   static const unsigned char nacl_msg[131] = {
      0x8e,0x99,0x3b,0x9f,0x48,0x68,0x12,0x73,
      0xc2,0x96,0x50,0xba,0x32,0xfc,0x76,0xce,
      0x48,0x33,0x2e,0xa7,0x16,0x4d,0x96,0xa4,
      0x47,0x6f,0xb8,0xc5,0x31,0xa1,0x18,0x6a,
      0xc0,0xdf,0xc1,0x7c,0x98,0xdc,0xe8,0x7b,
      0x4d,0xa7,0xf0,0x11,0xec,0x48,0xc9,0x72,
      0x71,0xd2,0xc2,0x0f,0x9b,0x92,0x8f,0xe2,
      0x27,0x0d,0x6f,0xb8,0x63,0xd5,0x17,0x38,
      0xb4,0x8e,0xee,0xe3,0x14,0xa7,0xcc,0x8a,
      0xb9,0x32,0x16,0x45,0x48,0xe5,0x26,0xae,
      0x90,0x22,0x43,0x68,0x51,0x7a,0xcf,0xea,
      0xbd,0x6b,0xb3,0x73,0x2b,0xc0,0xe9,0xda,
      0x99,0x83,0x2b,0x61,0xca,0x01,0xb6,0xde,
      0x56,0x24,0x4a,0x9e,0x88,0xd5,0xf9,0xb3,
      0x79,0x73,0xf6,0x22,0xa4,0x3d,0x14,0xa6,
      0x59,0x9b,0x1f,0x65,0x4c,0xb4,0x5a,0x74,
      0xe3,0x55,0xa5
   };

   static const unsigned char nacl_mac[16] = {
      0xf3,0xff,0xc7,0x70,0x3f,0x94,0x00,0xe5,
      0x2a,0x7d,0xfb,0x4b,0x3d,0x33,0x05,0xd9
   };

   /* generates a final value of (2^130 - 2) == 3 */
   static const unsigned char wrap_key[32] = {
      0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
   };

   static const unsigned char wrap_msg[16] = {
      0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
      0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
   };

   static const unsigned char wrap_mac[16] = {
      0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
      0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
   };

   /*
      mac of the macs of messages of length 0 to 256, where the key and messages
      have all their values set to the length
   */
   static const unsigned char total_key[32] = {
      0x01,0x02,0x03,0x04,0x05,0x06,0x07,
      0xff,0xfe,0xfd,0xfc,0xfb,0xfa,0xf9,
      0xff,0xff,0xff,0xff,0xff,0xff,0xff,
      0xff,0xff,0xff,0xff,0xff,0xff,0xff
   };

   static const unsigned char total_mac[16] = {
      0x64,0xaf,0xe2,0xe8,0xd6,0xad,0x7b,0xbd,
      0xd2,0x87,0xf9,0x7c,0x44,0x62,0x3d,0x39
   };

   poly1305_context ctx;
   poly1305_context total_ctx;
   unsigned char all_key[32];
   unsigned char all_msg[256];
   unsigned char mac[16];
   size_t i, j;
   int result = 1;

   for (i = 0; i < sizeof(mac); i++)
      mac[i] = 0;
   poly1305_auth(mac, nacl_msg, sizeof(nacl_msg), nacl_key);
   result &= poly1305_verify(nacl_mac, mac);

   for (i = 0; i < sizeof(mac); i++)
      mac[i] = 0;
   poly1305_init(&ctx, nacl_key);
   poly1305_update(&ctx, nacl_msg +   0, 32);
   poly1305_update(&ctx, nacl_msg +  32, 64);
   poly1305_update(&ctx, nacl_msg +  96, 16);
   poly1305_update(&ctx, nacl_msg + 112,  8);
   poly1305_update(&ctx, nacl_msg + 120,  4);
   poly1305_update(&ctx, nacl_msg + 124,  2);
   poly1305_update(&ctx, nacl_msg + 126,  1);
   poly1305_update(&ctx, nacl_msg + 127,  1);
   poly1305_update(&ctx, nacl_msg + 128,  1);
   poly1305_update(&ctx, nacl_msg + 129,  1);
   poly1305_update(&ctx, nacl_msg + 130,  1);
   poly1305_finish(&ctx, mac);
   result &= poly1305_verify(nacl_mac, mac);

   for (i = 0; i < sizeof(mac); i++)
      mac[i] = 0;
   poly1305_auth(mac, wrap_msg, sizeof(wrap_msg), wrap_key);
   result &= poly1305_verify(wrap_mac, mac);

   poly1305_init(&total_ctx, total_key);
   for (i = 0; i < 256; i++) {
      /* set key and message to 'i,i,i..' */
      for (j = 0; j < sizeof(all_key); j++)
         all_key[j] = i;
      for (j = 0; j < i; j++)
         all_msg[j] = i;
      poly1305_auth(mac, all_msg, i, all_key);
      poly1305_update(&total_ctx, mac, 16);
   }
   poly1305_finish(&total_ctx, mac);
   result &= poly1305_verify(total_mac, mac);

   return result;
}
