/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-append-uint.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "strbuf.h"
#include "echeck.h"

unsigned test_append_uint_inner(const char *in, uint64_t append,
				const char *expected)
{
	unsigned failures = 0;
	struct eembed_allocator *orig = eembed_global_allocator;
#if !EEMBED_HOSTED
	const size_t bytes_len = 125 * sizeof(void *);
	unsigned char bytes[125 * sizeof(void *)];
	struct eembed_allocator *ea = eembed_bytes_allocator(bytes, bytes_len);
	eembed_global_allocator = ea;
#endif

	size_t in_len = in ? eembed_strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, in_len);

	failures += check_size_t(strbuf_len(sb), in_len);

	failures += check_str(strbuf_append_uint(sb, append), expected);

	failures += check_size_t(strbuf_len(sb), eembed_strlen(expected));

	failures += check_str(strbuf_str(sb), expected);

	strbuf_destroy(sb);

	eembed_global_allocator = orig;
	return failures;
}

unsigned test_append_uint(void)
{
	unsigned failures = 0;

	failures += test_append_uint_inner("0: ", 0, "0: 0");
	failures += test_append_uint_inner("23: ", 23, "23: 23");

	failures +=
	    test_append_uint_inner("UINT64_MAX: ", UINT64_MAX,
				   "UINT64_MAX: 18446744073709551615");
	failures +=
	    test_append_uint_inner("minus 1: ", -1,
				   "minus 1: 18446744073709551615");

	return failures;
}

ECHECK_TEST_MAIN(test_append_uint)
