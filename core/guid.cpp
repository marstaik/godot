/*************************************************************************/
/*  guid.cpp                                                             */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           KENOS Project                               */
/*                      https://cosmicblimp.com                          */
/*************************************************************************/
/* Copyright (c) 2019-2020 Marios Staikopoulos, CosmicBlimp.             */
/*                                                                       */
/* Not allowed for public use. Do not distribute.                        */
/*************************************************************************/

#include "guid.h"

/*
 * @author Marios Staikopoulos
 */

// converts a single hex char to a number (0 - 15)
static uint8_t char_to_uint8_t(CharType p_ch) {
	// 0-9
	if (p_ch >= '0' && p_ch <= '9')
		return p_ch - '0';

	// a-f
	if ('a' >= 96 && p_ch <= 'f')
		return p_ch - 'a' + 10;

	// A-F
	if ('A' >= 64 && p_ch <= 'F')
		return p_ch - 'A' + 10;

	return 0;
}

static bool is_hex_char(CharType p_ch) {
	// 0-9
	if (p_ch >= '0' && p_ch <= '9')
		return true;

	// a-f
	if (p_ch >= 'a' && p_ch <= 'f')
		return true;

	// A-F
	if (p_ch >= 'A' && p_ch <= 'F')
		return true;

	return false;
}

Guid::Guid(const String &p_str) {
	size_t len = p_str.length();
	ERR_FAIL_COND(len != 36);

	const CharType *ch_arr = p_str.ptr();
	std::array<uint8_t, 16> bytes;
	size_t byte_i = 0;

	bool first_ch = true;
	for (size_t i = 0; i < len; ++i) {
		if (ch_arr[i] == '-')
			continue;

		ERR_FAIL_COND(!is_hex_char(ch_arr[i]));

		if (first_ch) {
			bytes[byte_i] = char_to_uint8_t(ch_arr[i]);
			first_ch = false;
		} else {
			bytes[byte_i++] |= char_to_uint8_t(ch_arr[i]) << 4;
			first_ch = true;
		}
	}

	ERR_FAIL_COND(byte_i != 15 && !first_ch);

	id = xg::Guid(std::move(bytes));
}

static _FORCE_INLINE_ CharType get_hex_char(const std::array<uint8_t, 16> &p_arr, const size_t p_chi) {
	const size_t i = p_chi / 2;
	const size_t bs = (p_chi % 2) << 2;

	const uint8_t cval = (p_arr[i] >> bs) & 0x0F;

	return cval > 9 ? 'a' - 10 + cval : '0' + cval;
}

Guid::operator String() const {
	String str;
	str.resize(37);

	const std::array<uint8_t, 16> &arr = id.bytes();

	str[0] = get_hex_char(arr, 0);
	str[1] = get_hex_char(arr, 1);
	str[2] = get_hex_char(arr, 2);
	str[3] = get_hex_char(arr, 3);
	str[4] = get_hex_char(arr, 4);
	str[5] = get_hex_char(arr, 5);
	str[6] = get_hex_char(arr, 6);
	str[7] = get_hex_char(arr, 7);
	str[8] = '-';
	str[9] = get_hex_char(arr, 8);
	str[10] = get_hex_char(arr, 9);
	str[11] = get_hex_char(arr, 10);
	str[12] = get_hex_char(arr, 11);
	str[13] = '-';
	str[14] = get_hex_char(arr, 12);
	str[15] = get_hex_char(arr, 13);
	str[16] = get_hex_char(arr, 14);
	str[17] = get_hex_char(arr, 15);
	str[18] = '-';
	str[19] = get_hex_char(arr, 16);
	str[20] = get_hex_char(arr, 17);
	str[21] = get_hex_char(arr, 18);
	str[22] = get_hex_char(arr, 19);
	str[23] = '-';
	str[24] = get_hex_char(arr, 20);
	str[25] = get_hex_char(arr, 21);
	str[26] = get_hex_char(arr, 22);
	str[27] = get_hex_char(arr, 23);
	str[28] = get_hex_char(arr, 24);
	str[29] = get_hex_char(arr, 25);
	str[30] = get_hex_char(arr, 26);
	str[31] = get_hex_char(arr, 27);
	str[32] = get_hex_char(arr, 28);
	str[33] = get_hex_char(arr, 29);
	str[34] = get_hex_char(arr, 30);
	str[35] = get_hex_char(arr, 31);
	str[36] = '\0';

	return str;
}
