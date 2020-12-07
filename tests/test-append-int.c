/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-append-int.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "strbuf.h"
#include "echeck.h"

unsigned test_append_int_inner(const char *in, int64_t append,
			       const char *expected)
{
	unsigned failures = 0;

	size_t in_len = in ? eembed_strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, in_len);

	failures += check_size_t(strbuf_len(sb), in_len);

	failures += check_str(strbuf_append_int(sb, append), expected);

	failures += check_size_t(strbuf_len(sb), eembed_strlen(expected));

	failures += check_str(strbuf_str(sb), expected);

	strbuf_destroy(sb);

	return failures;
}

unsigned test_append_int(void)
{
	unsigned failures = 0;

	failures += test_append_int_inner("0: ", 0, "0: 0");
	failures += test_append_int_inner("23: ", 23, "23: 23");
	failures += test_append_int_inner("minus 5: ", -5, "minus 5: -5");

	failures +=
	    test_append_int_inner("INT64_MAX: ", INT64_MAX,
				  "INT64_MAX: 9223372036854775807");
	failures +=
	    test_append_int_inner("INT64_MIN: ", INT64_MIN,
				  "INT64_MIN: -9223372036854775808");

	return failures;
}

ECHECK_TEST_MAIN(test_append_int)
