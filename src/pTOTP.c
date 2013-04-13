//Necessary for SHA1 calculation
#define _LITTLE_ENDIAN

#define OFFSET -7

//Update this to match your base32-encoded secret key
const char secretKey[] = "LUDNVCE536YNCDV3";

// Base32 implementation
//
// Copyright 2010 Google Inc.
// Author: Markus Gutschke
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>

// Base32 implementation
//
// Copyright 2010 Google Inc.
// Author: Markus Gutschke
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Encode and decode from base32 encoding using the following alphabet:
//   ABCDEFGHIJKLMNOPQRSTUVWXYZ234567
// This alphabet is documented in RFC 4668/3548
//
// We allow white-space and hyphens, but all other characters are considered
// invalid.
//
// All functions return the number of output bytes or -1 on error. If the
// output buffer is too small, the result will silently be truncated.

#ifndef _BASE32_H_
#define _BASE32_H_

#include <stdint.h>

int base32_decode(const uint8_t *encoded, uint8_t *result, int bufSize)
    __attribute__((visibility("hidden")));
int base32_encode(const uint8_t *data, int length, uint8_t *result,
                  int bufSize)
    __attribute__((visibility("hidden")));

#endif /* _BASE32_H_ */


int base32_decode(const uint8_t *encoded, uint8_t *result, int bufSize) {
  int buffer = 0;
  int bitsLeft = 0;
  int count = 0;
  for (const uint8_t *ptr = encoded; count < bufSize && *ptr; ++ptr) {
    uint8_t ch = *ptr;
    if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '-') {
      continue;
    }
    buffer <<= 5;

    // Deal with commonly mistyped characters
    if (ch == '0') {
      ch = 'O';
    } else if (ch == '1') {
      ch = 'L';
    } else if (ch == '8') {
      ch = 'B';
    }

    // Look up one base32 digit
    if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
      ch = (ch & 0x1F) - 1;
    } else if (ch >= '2' && ch <= '7') {
      ch -= '2' - 26;
    } else {
      return -1;
    }

    buffer |= ch;
    bitsLeft += 5;
    if (bitsLeft >= 8) {
      result[count++] = buffer >> (bitsLeft - 8);
      bitsLeft -= 8;
    }
  }
  if (count < bufSize) {
    result[count] = '\000';
  }
  return count;
}

int base32_encode(const uint8_t *data, int length, uint8_t *result,
                  int bufSize) {
  if (length < 0 || length > (1 << 28)) {
    return -1;
  }
  int count = 0;
  if (length > 0) {
    int buffer = data[0];
    int next = 1;
    int bitsLeft = 8;
    while (count < bufSize && (bitsLeft > 0 || next < length)) {
      if (bitsLeft < 5) {
        if (next < length) {
          buffer <<= 8;
          buffer |= data[next++] & 0xFF;
          bitsLeft += 8;
        } else {
          int pad = 5 - bitsLeft;
          buffer <<= pad;
          bitsLeft += pad;
        }
      }
      int index = 0x1F & (buffer >> (bitsLeft - 5));
      bitsLeft -= 5;
      result[count++] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"[index];
    }
  }
  if (count < bufSize) {
    result[count] = '\000';
  }
  return count;
}
// HMAC_SHA1 implementation
//
// Copyright 2010 Google Inc.
// Author: Markus Gutschke
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <string.h>

// HMAC_SHA1 implementation
//
// Copyright 2010 Google Inc.
// Author: Markus Gutschke
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _HMAC_H_
#define _HMAC_H_

#include <stdint.h>

void hmac_sha1(const uint8_t *key, int keyLength,
               const uint8_t *data, int dataLength,
               uint8_t *result, int resultLength)
 __attribute__((visibility("hidden")));

#endif /* _HMAC_H_ */
// SHA1 header file
//
// Copyright 2010 Google Inc.
// Author: Markus Gutschke
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SHA1_H__
#define SHA1_H__

#include <stdint.h>

#define SHA1_BLOCKSIZE     64
#define SHA1_DIGEST_LENGTH 20

typedef struct {
  uint32_t digest[8];
  uint32_t count_lo, count_hi;
  uint8_t  data[SHA1_BLOCKSIZE];
  int      local;
} SHA1_INFO;

void sha1_init(SHA1_INFO *sha1_info) __attribute__((visibility("hidden")));
void sha1_update(SHA1_INFO *sha1_info, const uint8_t *buffer, int count)
  __attribute__((visibility("hidden")));
void sha1_final(SHA1_INFO *sha1_info, uint8_t digest[20])
  __attribute__((visibility("hidden")));

#endif


void hmac_sha1(const uint8_t *key, int keyLength,
               const uint8_t *data, int dataLength,
               uint8_t *result, int resultLength) {
  SHA1_INFO ctx;
  uint8_t hashed_key[SHA1_DIGEST_LENGTH];
  if (keyLength > 64) {
    // The key can be no bigger than 64 bytes. If it is, we'll hash it down to
    // 20 bytes.
    sha1_init(&ctx);
    sha1_update(&ctx, key, keyLength);
    sha1_final(&ctx, hashed_key);
    key = hashed_key;
    keyLength = SHA1_DIGEST_LENGTH;
  }

  // The key for the inner digest is derived from our key, by padding the key
  // the full length of 64 bytes, and then XOR'ing each byte with 0x36.
  uint8_t tmp_key[64];
  for (int i = 0; i < keyLength; ++i) {
    tmp_key[i] = key[i] ^ 0x36;
  }
  memset(tmp_key + keyLength, 0x36, 64 - keyLength);

  // Compute inner digest
  sha1_init(&ctx);
  sha1_update(&ctx, tmp_key, 64);
  sha1_update(&ctx, data, dataLength);
  uint8_t sha[SHA1_DIGEST_LENGTH];
  sha1_final(&ctx, sha);

  // The key for the outer digest is derived from our key, by padding the key
  // the full length of 64 bytes, and then XOR'ing each byte with 0x5C.
  for (int i = 0; i < keyLength; ++i) {
    tmp_key[i] = key[i] ^ 0x5C;
  }
  memset(tmp_key + keyLength, 0x5C, 64 - keyLength);

  // Compute outer digest
  sha1_init(&ctx);
  sha1_update(&ctx, tmp_key, 64);
  sha1_update(&ctx, sha, SHA1_DIGEST_LENGTH);
  sha1_final(&ctx, sha);

  // Copy result to output buffer and truncate or pad as necessary
  memset(result, 0, resultLength);
  if (resultLength > SHA1_DIGEST_LENGTH) {
    resultLength = SHA1_DIGEST_LENGTH;
  }
  memcpy(result, sha, resultLength);

  // Zero out all internal data structures
  memset(hashed_key, 0, sizeof(hashed_key));
  memset(sha, 0, sizeof(sha));
  memset(tmp_key, 0, sizeof(tmp_key));
}
/*
 * Copyright 2010 Google Inc.
 * Author: Markus Gutschke
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 *
 * An earlier version of this file was originally released into the public
 * domain by its authors. It has been modified to make the code compile and
 * link as part of the Google Authenticator project. These changes are
 * copyrighted by Google Inc. and released under the Apache License,
 * Version 2.0.
 *
 * The previous authors' terms are included below:
 */

/*****************************************************************************
 *
 * File:    sha1.c
 *
 * Purpose: Implementation of the SHA1 message-digest algorithm.
 *
 * NIST Secure Hash Algorithm
 *   Heavily modified by Uwe Hollerbach <uh@alumni.caltech edu>
 *   from Peter C. Gutmann's implementation as found in
 *   Applied Cryptography by Bruce Schneier
 *   Further modifications to include the "UNRAVEL" stuff, below
 *
 * This code is in the public domain
 *
 *****************************************************************************
*/
#define _BSD_SOURCE
#include <sys/types.h> // Defines BYTE_ORDER, iff _BSD_SOURCE is defined
#include <string.h>

// SHA1 header file
//
// Copyright 2010 Google Inc.
// Author: Markus Gutschke
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SHA1_H__
#define SHA1_H__

#include <stdint.h>

#define SHA1_BLOCKSIZE     64
#define SHA1_DIGEST_LENGTH 20

typedef struct {
  uint32_t digest[8];
  uint32_t count_lo, count_hi;
  uint8_t  data[SHA1_BLOCKSIZE];
  int      local;
} SHA1_INFO;

void sha1_init(SHA1_INFO *sha1_info) __attribute__((visibility("hidden")));
void sha1_update(SHA1_INFO *sha1_info, const uint8_t *buffer, int count)
  __attribute__((visibility("hidden")));
void sha1_final(SHA1_INFO *sha1_info, uint8_t digest[20])
  __attribute__((visibility("hidden")));

#endif


#if !defined(BYTE_ORDER)
#if defined(_BIG_ENDIAN)
#define BYTE_ORDER 4321
#elif defined(_LITTLE_ENDIAN)
#define BYTE_ORDER 1234
#else
#error Need to define BYTE_ORDER
#endif
#endif

#ifndef TRUNC32
  #define TRUNC32(x)  ((x) & 0xffffffffL)
#endif

/* SHA f()-functions */
#define f1(x,y,z)    ((x & y) | (~x & z))
#define f2(x,y,z)    (x ^ y ^ z)
#define f3(x,y,z)    ((x & y) | (x & z) | (y & z))
#define f4(x,y,z)    (x ^ y ^ z)

/* SHA constants */
#define CONST1        0x5a827999L
#define CONST2        0x6ed9eba1L
#define CONST3        0x8f1bbcdcL
#define CONST4        0xca62c1d6L

/* truncate to 32 bits -- should be a null op on 32-bit machines */
#define T32(x)    ((x) & 0xffffffffL)

/* 32-bit rotate */
#define R32(x,n)    T32(((x << n) | (x >> (32 - n))))

/* the generic case, for when the overall rotation is not unraveled */
#define FG(n)    \
    T = T32(R32(A,5) + f##n(B,C,D) + E + *WP++ + CONST##n);    \
    E = D; D = C; C = R32(B,30); B = A; A = T

/* specific cases, for when the overall rotation is unraveled */
#define FA(n)    \
    T = T32(R32(A,5) + f##n(B,C,D) + E + *WP++ + CONST##n); B = R32(B,30)

#define FB(n)    \
    E = T32(R32(T,5) + f##n(A,B,C) + D + *WP++ + CONST##n); A = R32(A,30)

#define FC(n)    \
    D = T32(R32(E,5) + f##n(T,A,B) + C + *WP++ + CONST##n); T = R32(T,30)

#define FD(n)    \
    C = T32(R32(D,5) + f##n(E,T,A) + B + *WP++ + CONST##n); E = R32(E,30)

#define FE(n)    \
    B = T32(R32(C,5) + f##n(D,E,T) + A + *WP++ + CONST##n); D = R32(D,30)

#define FT(n)    \
    A = T32(R32(B,5) + f##n(C,D,E) + T + *WP++ + CONST##n); C = R32(C,30)


static void
sha1_transform(SHA1_INFO *sha1_info)
{
    int i;
    uint8_t *dp;
    uint32_t T, A, B, C, D, E, W[80], *WP;

    dp = sha1_info->data;

#undef SWAP_DONE

#if BYTE_ORDER == 1234
#define SWAP_DONE
    for (i = 0; i < 16; ++i) {
        T = *((uint32_t *) dp);
        dp += 4;
        W[i] = 
            ((T << 24) & 0xff000000) |
            ((T <<  8) & 0x00ff0000) |
            ((T >>  8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
    }
#endif

#if BYTE_ORDER == 4321
#define SWAP_DONE
    for (i = 0; i < 16; ++i) {
        T = *((uint32_t *) dp);
        dp += 4;
        W[i] = TRUNC32(T);
    }
#endif

#if BYTE_ORDER == 12345678
#define SWAP_DONE
    for (i = 0; i < 16; i += 2) {
        T = *((uint32_t *) dp);
        dp += 8;
        W[i] =  ((T << 24) & 0xff000000) | ((T <<  8) & 0x00ff0000) |
            ((T >>  8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
        T >>= 32;
        W[i+1] = ((T << 24) & 0xff000000) | ((T <<  8) & 0x00ff0000) |
            ((T >>  8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
    }
#endif

#if BYTE_ORDER == 87654321
#define SWAP_DONE
    for (i = 0; i < 16; i += 2) {
        T = *((uint32_t *) dp);
        dp += 8;
        W[i] = TRUNC32(T >> 32);
        W[i+1] = TRUNC32(T);
    }
#endif

#ifndef SWAP_DONE
#define SWAP_DONE
    for (i = 0; i < 16; ++i) {
        T = *((uint32_t *) dp);
        dp += 4;
        W[i] = TRUNC32(T);
    }
#endif /* SWAP_DONE */

    for (i = 16; i < 80; ++i) {
    W[i] = W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16];
    W[i] = R32(W[i], 1);
    }
    A = sha1_info->digest[0];
    B = sha1_info->digest[1];
    C = sha1_info->digest[2];
    D = sha1_info->digest[3];
    E = sha1_info->digest[4];
    WP = W;
#ifdef UNRAVEL
    FA(1); FB(1); FC(1); FD(1); FE(1); FT(1); FA(1); FB(1); FC(1); FD(1);
    FE(1); FT(1); FA(1); FB(1); FC(1); FD(1); FE(1); FT(1); FA(1); FB(1);
    FC(2); FD(2); FE(2); FT(2); FA(2); FB(2); FC(2); FD(2); FE(2); FT(2);
    FA(2); FB(2); FC(2); FD(2); FE(2); FT(2); FA(2); FB(2); FC(2); FD(2);
    FE(3); FT(3); FA(3); FB(3); FC(3); FD(3); FE(3); FT(3); FA(3); FB(3);
    FC(3); FD(3); FE(3); FT(3); FA(3); FB(3); FC(3); FD(3); FE(3); FT(3);
    FA(4); FB(4); FC(4); FD(4); FE(4); FT(4); FA(4); FB(4); FC(4); FD(4);
    FE(4); FT(4); FA(4); FB(4); FC(4); FD(4); FE(4); FT(4); FA(4); FB(4);
    sha1_info->digest[0] = T32(sha1_info->digest[0] + E);
    sha1_info->digest[1] = T32(sha1_info->digest[1] + T);
    sha1_info->digest[2] = T32(sha1_info->digest[2] + A);
    sha1_info->digest[3] = T32(sha1_info->digest[3] + B);
    sha1_info->digest[4] = T32(sha1_info->digest[4] + C);
#else /* !UNRAVEL */
#ifdef UNROLL_LOOPS
    FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1);
    FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1); FG(1);
    FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2);
    FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2); FG(2);
    FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3);
    FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3); FG(3);
    FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4);
    FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4); FG(4);
#else /* !UNROLL_LOOPS */
    for (i =  0; i < 20; ++i) { FG(1); }
    for (i = 20; i < 40; ++i) { FG(2); }
    for (i = 40; i < 60; ++i) { FG(3); }
    for (i = 60; i < 80; ++i) { FG(4); }
#endif /* !UNROLL_LOOPS */
    sha1_info->digest[0] = T32(sha1_info->digest[0] + A);
    sha1_info->digest[1] = T32(sha1_info->digest[1] + B);
    sha1_info->digest[2] = T32(sha1_info->digest[2] + C);
    sha1_info->digest[3] = T32(sha1_info->digest[3] + D);
    sha1_info->digest[4] = T32(sha1_info->digest[4] + E);
#endif /* !UNRAVEL */
}

/* initialize the SHA digest */

void
sha1_init(SHA1_INFO *sha1_info)
{
    sha1_info->digest[0] = 0x67452301L;
    sha1_info->digest[1] = 0xefcdab89L;
    sha1_info->digest[2] = 0x98badcfeL;
    sha1_info->digest[3] = 0x10325476L;
    sha1_info->digest[4] = 0xc3d2e1f0L;
    sha1_info->count_lo = 0L;
    sha1_info->count_hi = 0L;
    sha1_info->local = 0;
}

/* update the SHA digest */

void
sha1_update(SHA1_INFO *sha1_info, const uint8_t *buffer, int count)
{
    int i;
    uint32_t clo;

    clo = T32(sha1_info->count_lo + ((uint32_t) count << 3));
    if (clo < sha1_info->count_lo) {
    ++sha1_info->count_hi;
    }
    sha1_info->count_lo = clo;
    sha1_info->count_hi += (uint32_t) count >> 29;
    if (sha1_info->local) {
    i = SHA1_BLOCKSIZE - sha1_info->local;
    if (i > count) {
        i = count;
    }
    memcpy(((uint8_t *) sha1_info->data) + sha1_info->local, buffer, i);
    count -= i;
    buffer += i;
    sha1_info->local += i;
    if (sha1_info->local == SHA1_BLOCKSIZE) {
        sha1_transform(sha1_info);
    } else {
        return;
    }
    }
    while (count >= SHA1_BLOCKSIZE) {
    memcpy(sha1_info->data, buffer, SHA1_BLOCKSIZE);
    buffer += SHA1_BLOCKSIZE;
    count -= SHA1_BLOCKSIZE;
    sha1_transform(sha1_info);
    }
    memcpy(sha1_info->data, buffer, count);
    sha1_info->local = count;
}


static void
sha1_transform_and_copy(unsigned char digest[20], SHA1_INFO *sha1_info)
{
    sha1_transform(sha1_info);
    digest[ 0] = (unsigned char) ((sha1_info->digest[0] >> 24) & 0xff);
    digest[ 1] = (unsigned char) ((sha1_info->digest[0] >> 16) & 0xff);
    digest[ 2] = (unsigned char) ((sha1_info->digest[0] >>  8) & 0xff);
    digest[ 3] = (unsigned char) ((sha1_info->digest[0]      ) & 0xff);
    digest[ 4] = (unsigned char) ((sha1_info->digest[1] >> 24) & 0xff);
    digest[ 5] = (unsigned char) ((sha1_info->digest[1] >> 16) & 0xff);
    digest[ 6] = (unsigned char) ((sha1_info->digest[1] >>  8) & 0xff);
    digest[ 7] = (unsigned char) ((sha1_info->digest[1]      ) & 0xff);
    digest[ 8] = (unsigned char) ((sha1_info->digest[2] >> 24) & 0xff);
    digest[ 9] = (unsigned char) ((sha1_info->digest[2] >> 16) & 0xff);
    digest[10] = (unsigned char) ((sha1_info->digest[2] >>  8) & 0xff);
    digest[11] = (unsigned char) ((sha1_info->digest[2]      ) & 0xff);
    digest[12] = (unsigned char) ((sha1_info->digest[3] >> 24) & 0xff);
    digest[13] = (unsigned char) ((sha1_info->digest[3] >> 16) & 0xff);
    digest[14] = (unsigned char) ((sha1_info->digest[3] >>  8) & 0xff);
    digest[15] = (unsigned char) ((sha1_info->digest[3]      ) & 0xff);
    digest[16] = (unsigned char) ((sha1_info->digest[4] >> 24) & 0xff);
    digest[17] = (unsigned char) ((sha1_info->digest[4] >> 16) & 0xff);
    digest[18] = (unsigned char) ((sha1_info->digest[4] >>  8) & 0xff);
    digest[19] = (unsigned char) ((sha1_info->digest[4]      ) & 0xff);
}

/* finish computing the SHA digest */
void
sha1_final(SHA1_INFO *sha1_info, uint8_t digest[20])
{
    int count;
    uint32_t lo_bit_count, hi_bit_count;

    lo_bit_count = sha1_info->count_lo;
    hi_bit_count = sha1_info->count_hi;
    count = (int) ((lo_bit_count >> 3) & 0x3f);
    ((uint8_t *) sha1_info->data)[count++] = 0x80;
    if (count > SHA1_BLOCKSIZE - 8) {
    memset(((uint8_t *) sha1_info->data) + count, 0, SHA1_BLOCKSIZE - count);
    sha1_transform(sha1_info);
    memset((uint8_t *) sha1_info->data, 0, SHA1_BLOCKSIZE - 8);
    } else {
    memset(((uint8_t *) sha1_info->data) + count, 0,
        SHA1_BLOCKSIZE - 8 - count);
    }
    sha1_info->data[56] = (uint8_t)((hi_bit_count >> 24) & 0xff);
    sha1_info->data[57] = (uint8_t)((hi_bit_count >> 16) & 0xff);
    sha1_info->data[58] = (uint8_t)((hi_bit_count >>  8) & 0xff);
    sha1_info->data[59] = (uint8_t)((hi_bit_count >>  0) & 0xff);
    sha1_info->data[60] = (uint8_t)((lo_bit_count >> 24) & 0xff);
    sha1_info->data[61] = (uint8_t)((lo_bit_count >> 16) & 0xff);
    sha1_info->data[62] = (uint8_t)((lo_bit_count >>  8) & 0xff);
    sha1_info->data[63] = (uint8_t)((lo_bit_count >>  0) & 0xff);
    sha1_transform_and_copy(digest, sha1_info);
}

/***EOF***/
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

//#include "libpam/base32.h"
//#include "libpam/hmac.h"
//#include "libpam/sha1.h"

#define SECRET                    "/.google_authenticator"
#define SECRET_BITS               80          // Must be divisible by eight
#define VERIFICATION_CODE_MODULUS (1000*1000) // Six digits
#define SCRATCHCODES              5           // Number of initial scratchcodes
#define SCRATCHCODE_LENGTH        8           // Eight digits per scratchcode
#define BYTES_PER_SCRATCHCODE     4           // 32bit of randomness is enough
#define BITS_PER_BASE32_CHAR      5           // Base32 expands space by 8/5

static int generateCode(const char *key, unsigned long tm) {
  uint8_t challenge[8];
  for (int i = 8; i--; tm >>= 8) {
    challenge[i] = tm;
  }

  // Estimated number of bytes needed to represent the decoded secret. Because
  // of white-space and separators, this is an upper bound of the real number,
  // which we later get as a return-value from base32_decode()
  int secretLen = (strlen(key) + 7)/8*BITS_PER_BASE32_CHAR;

  // Sanity check, that our secret will fixed into a reasonably-sized static
  // array.
  if (secretLen <= 0 || secretLen > 100) {
    return -1;
  }

  // Decode secret from Base32 to a binary representation, and check that we
  // have at least one byte's worth of secret data.
  uint8_t secret[100];
  if ((secretLen = base32_decode((const uint8_t *)key, secret, secretLen))<1) {
    return -1;
  }

  // Compute the HMAC_SHA1 of the secrete and the challenge.
  uint8_t hash[SHA1_DIGEST_LENGTH];
  hmac_sha1(secret, secretLen, challenge, 8, hash, SHA1_DIGEST_LENGTH);

  // Pick the offset where to sample our hash value for the actual verification
  // code.
  int offset = hash[SHA1_DIGEST_LENGTH - 1] & 0xF;

  // Compute the truncated hash in a byte-order independent loop.
  unsigned int truncatedHash = 0;
  for (int i = 0; i < 4; ++i) {
    truncatedHash <<= 8;
    truncatedHash  |= hash[offset + i];
  }

  // Truncate to a smaller number of digits.
  truncatedHash &= 0x7FFFFFFF;
  truncatedHash %= VERIFICATION_CODE_MODULUS;

  return truncatedHash;
}

//Thanks to Hexxeh for this one!
unsigned int get_unix_time(int tm_tz)
{
  PblTm curr_time;
  get_time(&curr_time);
  unsigned int now = 0;
  now += (curr_time.tm_year-70)*31536000;
  now += ((curr_time.tm_year-69)/4)*86400;
  now -= ((curr_time.tm_year-1)/100)*86400;
  now += ((curr_time.tm_year+299)/400)*86400;
  now += curr_time.tm_yday*86400;
  now += (curr_time.tm_hour+(tm_tz))*3600;
  now += curr_time.tm_min*60;
  now += curr_time.tm_sec;
  return now;
}


#define MY_UUID { 0xF3, 0x61, 0x70, 0x30, 0x87, 0x06, 0x43, 0xB6, 0xAD, 0xDE, 0xD7, 0x3F, 0xDB, 0x38, 0x02, 0x44 }
PBL_APP_INFO(MY_UUID,
             "OTP", "Public Domain",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;

TextLayer textLayer;

int oldCode = -1;
unsigned int genTime;
unsigned int lastTick;

//                                 1           2
//                      0123455678901223456789901223456788
char displayBuffer[] = "TOTPb\n000000\nGMT-12\n30\nGmail\0";
int tz_offset = OFFSET;

void redraw() {
  int code = oldCode;

  if(code == -1) {
    displayBuffer[6] = ' ';
    displayBuffer[7] = ' ';
    displayBuffer[8] = ' ';
    displayBuffer[9] = ' ';
    displayBuffer[10] = ' ';
    displayBuffer[11] = ' ';

    displayBuffer[20] = ' ';
    displayBuffer[21] = ' ';
  }
  else {
    for(int x=0;x<6;x++) {
      displayBuffer[11-x] = '0'+(code % 10);
      code /= 10;
    }

    displayBuffer[20] = '0' + lastTick / 10;
    displayBuffer[21] = '0' + lastTick % 10;
  }

  int tzo = tz_offset;
  if(tzo < 0) {
    tzo = -tzo;
    displayBuffer[16] = '-';
  } else {
    displayBuffer[16] = '+';
  }

  if(tzo > 9) {
    displayBuffer[17] = '1';
    tzo -= 10;
  } else {
    displayBuffer[17] = '0';
  }

  displayBuffer[18] = '0' + tzo;

  text_layer_set_text(&textLayer, displayBuffer);
}

bool recode() {
  const char *key = secretKey;

  genTime = get_unix_time(0);
  lastTick = 30 - genTime % 30;
  genTime /= 30;

  unsigned int cTime = get_unix_time(-tz_offset);

  unsigned int quantized_time = cTime/30;


  int code = generateCode(key, quantized_time);

  if(oldCode != code) {
    oldCode = code;
    return true;
  }
  return false;
}

// Modify these common button handlers

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(tz_offset < 12) {
    tz_offset++;
    recode();
    redraw();
  }

}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(tz_offset > -12) {
    tz_offset--;
    recode();
    redraw();
  }
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

  if(recode())
    redraw();
}


void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;

}


// This usually won't need to be modified

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;

  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}


// Standard app initialisation

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Button App");
  window_stack_push(&window, true /* Animated */);

  text_layer_init(&textLayer, window.layer.frame);
  text_layer_set_text(&textLayer, "Hello World");
  text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK));
  layer_add_child(&window.layer, &textLayer.layer);

  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
}

void handle_tick(AppContextRef ctx, PebbleTickEvent *event) {
  (void)ctx;
  (void)event;

  unsigned int curTime = get_unix_time(0);
  lastTick = 30 - curTime % 30;
  curTime /= 30;

  if(genTime != curTime) {
    oldCode = -1;
  }

  redraw();
}


void pbl_main(void *params) {

  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_tick,
      .tick_units = SECOND_UNIT
    }
  };
  app_event_loop(params, &handlers);
}