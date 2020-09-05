/*
Copyright (C) 2020 by Sebastian Kauertz.

This file is part of Timekeeper, a Qt-based time tracking app.

Timekeeper is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Timekeeper is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <https://www.gnu.org/licenses/>.
*/
#include <sys/types.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "crypto/pbkdf2.h"
#include "crypto/SHA1.h"

// Adapted from: http://bxr.su/OpenBSD/lib/libutil/pkcs5_pbkdf2.c
// (Apparently the best I could find...)

/*
 * HMAC-SHA-1 (from RFC 2202).
 */
static void hmac_sha1(const uint8_t *text, size_t text_len, const uint8_t *key, size_t key_len, uint8_t digest[SHA1_DIGEST_LENGTH])
{
    uint8_t k_pad[SHA1_BLOCK_LENGTH];
    uint8_t tk[SHA1_DIGEST_LENGTH];
    int i;
    SHA1 sha1;

    if (key_len > SHA1_BLOCK_LENGTH) {
        sha1.reset();
        sha1.update(key, key_len);
        sha1.finalize(tk, SHA1_DIGEST_LENGTH);

        key = tk;
        key_len = SHA1_DIGEST_LENGTH;
    }

    memset(k_pad, 0, sizeof(k_pad));
    memcpy(k_pad, key, key_len);
    for (i = 0; i < SHA1_BLOCK_LENGTH; i++)
        k_pad[i] ^= 0x36;

    sha1.reset();
    sha1.update(k_pad, SHA1_BLOCK_LENGTH);
    sha1.update(text, text_len);
    sha1.finalize(digest, SHA1_DIGEST_LENGTH);

    memset(k_pad, 0, sizeof(k_pad));
    memcpy(k_pad, key, key_len);
    for (i = 0; i < SHA1_BLOCK_LENGTH; i++)
        k_pad[i] ^= 0x5c;

    sha1.reset();
    sha1.update(k_pad, SHA1_BLOCK_LENGTH);
    sha1.update(digest, SHA1_DIGEST_LENGTH);
    sha1.finalize(digest, SHA1_DIGEST_LENGTH);
}


/*
 * Password-Based Key Derivation Function 2 (PKCS #5 v2.0).
 * Code based on IEEE Std 802.11-2007, Annex H.4.2.
 */
int pkcs5_pbkdf2(const uint8_t *pass, size_t pass_len, const uint8_t *salt, size_t salt_len, uint8_t *key, size_t key_len, unsigned int rounds)
{
    uint8_t *asalt, obuf[SHA1_DIGEST_LENGTH];
    uint8_t d1[SHA1_DIGEST_LENGTH], d2[SHA1_DIGEST_LENGTH];
    unsigned int i, j;
    unsigned int count;
    size_t r;


    if (rounds < 1 || key_len == 0) {
        memset(key, 0, key_len);
        return -1;
    }

    if (salt_len == 0 || salt_len > SIZE_MAX - 4) {
        memset(key, 0, key_len);
        return -1;
    }

    asalt = new uint8_t[salt_len + 4];
    if (asalt == NULL) {
        memset(key, 0, key_len);
        return -1;
    }

    memcpy(asalt, salt, salt_len);

    for (count = 1; key_len > 0; count++) {
        asalt[salt_len + 0] = (count >> 24) & 0xff;
        asalt[salt_len + 1] = (count >> 16) & 0xff;
        asalt[salt_len + 2] = (count >> 8) & 0xff;
        asalt[salt_len + 3] = count & 0xff;
        hmac_sha1(asalt, salt_len + 4, pass, pass_len, d1);
        memcpy(obuf, d1, sizeof(obuf));

        for (i = 1; i < rounds; i++) {
            hmac_sha1(d1, sizeof(d1), pass, pass_len, d2);
            memcpy(d1, d2, sizeof(d1));
            for (j = 0; j < sizeof(obuf); j++)
                obuf[j] ^= d1[j];
        }

        r = MINIMUM(key_len, SHA1_DIGEST_LENGTH);
        memcpy(key, obuf, r);
        key += r;
        key_len -= r;
    };

    delete[] asalt;

//    explicit_bzero(d1, sizeof(d1));
//    explicit_bzero(d2, sizeof(d2));
//    explicit_bzero(obuf, sizeof(obuf));

    return 0;

}
