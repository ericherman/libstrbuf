/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-avail.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "strbuf.h"
#include "echeck.h"

unsigned test_avail(void)
{
	unsigned failures = 0;

	/* sizeof(strbuf_s) is about 64 */
	/* we expect only about 16 bytes left to make use of a string data */
	size_t buf_size = 80;
	unsigned char buf[buf_size];
	strbuf_s *sb = strbuf_no_grow(buf, buf_size, NULL, 0);

	size_t usable = strbuf_avail(sb);
	failures += check_int(usable > 0 ? 1 : 0, 1);
	failures += check_int(usable < buf_size ? 1 : 0, 1);

	const char *abc_ws = "   abc   ";
	const char *rv1 = strbuf_set(sb, abc_ws, eembed_strlen(abc_ws));
	check_int(rv1 == NULL ? 0 : 1, 1);
	const char *rv2 = strbuf_trim(sb);
	check_int(rv2 == NULL ? 0 : 1, 1);

	size_t expected_remaining = usable - 3;
	size_t remaining = strbuf_avail(sb);
	failures += check_int(remaining, expected_remaining);

	char junk[3 + remaining + 1];
	junk[0] = 'a';
	junk[1] = 'b';
	junk[2] = 'c';
	eembed_memset(junk + 3, 'X', remaining);
	junk[3 + remaining] = '\0';

	const char *res = strbuf_append(sb, junk + 3, eembed_strlen(junk + 3));
	failures += check_str(res, junk);
	failures += check_str(strbuf_str(sb), junk);

	const char *xyz_ws = "   xyz   ";
	strbuf_set(sb, xyz_ws, eembed_strlen(xyz_ws));
	strbuf_trim(sb);
	expected_remaining = usable - 3;
	remaining = strbuf_avail(sb);
	failures += check_int(remaining, expected_remaining);

	eembed_memset(junk, 'Q', remaining);
	junk[remaining] = '\0';
	const char *res2 = strbuf_prepend(sb, junk, eembed_strlen(junk));
	junk[remaining + 0] = 'x';
	junk[remaining + 1] = 'y';
	junk[remaining + 2] = 'z';
	junk[remaining + 3] = '\0';
	failures += check_str(res2, junk);
	failures += check_str(strbuf_str(sb), junk);

	strbuf_destroy(sb);

	return failures;
}

ECHECK_TEST_MAIN(test_avail)
