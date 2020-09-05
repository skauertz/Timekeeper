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
#ifndef PBKDF2_H
#define PBKDF2_H

#define	MINIMUM(a,b) (((a) < (b)) ? (a) : (b))
#define SHA1_BLOCK_LENGTH 64
#define SHA1_DIGEST_LENGTH 20

static void hmac_sha1(const uint8_t *text, size_t text_len, const uint8_t *key, size_t key_len, uint8_t digest[SHA1_DIGEST_LENGTH]);
int pkcs5_pbkdf2(const uint8_t *pass, size_t pass_len, const uint8_t *salt, size_t salt_len, uint8_t *key, size_t key_len, unsigned int rounds);


#endif // PBKDF2_H
