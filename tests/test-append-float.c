/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-append-float.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "strbuf.h"
#include "echeck.h"

int test_append_float_inner(const char *in, long double append,
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

	failures += check_str(strbuf_append_float(sb, append), expected);

	failures += check_size_t(strbuf_len(sb), eembed_strlen(expected));

	failures += check_str(strbuf_str(sb), expected);

	strbuf_destroy(sb);

	eembed_global_allocator = orig;
	return failures;
}

#if EEMBED_HOSTED
#include <stdio.h>
#endif

unsigned test_append_float(void)
{
	unsigned failures = 0;

	failures += test_append_float_inner("foo: ", 1.2, "foo: 1.2");
	failures +=
	    test_append_float_inner("bar: ", -1.0 / 3.0, "bar: -0.333333");

#if EEMBED_HOSTED
	failures +=
	    test_append_float_inner("baz: ", 6000000000.0, "baz: 6e+09");

	char expected[80];
	snprintf(expected, 80, "-LDBL_MIN: %Lg", -LDBL_MIN);
	failures += test_append_float_inner("-LDBL_MIN: ", -LDBL_MIN, expected);
#endif

	return failures;
}

ECHECK_TEST_MAIN(test_append_float)
