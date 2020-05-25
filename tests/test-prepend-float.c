/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-prepend-float.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include <strbuf.h>
#include <echeck.h>

#include <assert.h>
#include <string.h>

int test_prepend_float(const char *in, long double prepend,
		       const char *expected)
{
	size_t in_len = in ? strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, in_len);

	int failures = 0;

	failures += check_size_t(strbuf_len(sb), in_len);

	failures += check_str(strbuf_prepend_float(sb, prepend), expected);

	failures += check_size_t(strbuf_len(sb), strlen(expected));

	failures += check_str(strbuf_str(sb), expected);

	strbuf_destroy(sb);

	return failures;
}

int main(int argc, char **argv)
{
	int failures = 0;
	assert(argc);
	assert(argv);

	failures += test_prepend_float(" foo", 1.2, "1.2 foo");
	failures += test_prepend_float(" bar", -1.0 / 3.0, "-0.333333 bar");
	failures += test_prepend_float(" baz", 6000000000.0, "6e+09 baz");

	char expected[80];
	snprintf(expected, 80, "%Lg -LDBL_MIN", -LDBL_MIN);
	failures += test_prepend_float(" -LDBL_MIN", -LDBL_MIN, expected);

	if (failures) {
		fprintf(stderr, "%d failures in %s\n", failures, __FILE__);
	}

	return failures ? 1 : 0;
}
