/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-prepend-float.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "strbuf.h"
#include "echeck.h"

unsigned test_prepend_float_inner(const char *in, long double prepend,
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

	failures += check_str(strbuf_prepend_float(sb, prepend), expected);

	failures += check_size_t(strbuf_len(sb), eembed_strlen(expected));

	failures += check_str(strbuf_str(sb), expected);

	strbuf_destroy(sb);

	eembed_global_allocator = orig;
	return failures;
}

#if EEMBED_HOSTED
#include <stdio.h>
#endif

unsigned test_prepend_float(void)
{
	unsigned failures = 0;

	failures += test_prepend_float_inner(" foo", 1.2, "1.2 foo");
	failures +=
	    test_prepend_float_inner(" bar", -1.0 / 3.0, "-0.333333 bar");

#if EEMBED_HOSTED
	failures += test_prepend_float_inner(" baz", 6000000000.0, "6e+09 baz");

	/*
	   3.3621e-4932  LDBL_MIN       16
	   -3.3621e-4932        -LDBL_MIN       16
	   1.79769e+308  DBL_MAX        8
	   -2.22507e-308        -DBL_MIN        8
	   3.40282e+38   FLT_MAX        4
	   -1.17549e-38 -FLT_MIN        4
	 */

	char expected[80];
	snprintf(expected, 80, "%Lg -LDBL_MIN", -LDBL_MIN);
	failures += test_prepend_float_inner(" -LDBL_MIN", -LDBL_MIN, expected);
#endif

	return failures;
}

ECHECK_TEST_MAIN(test_prepend_float)
