/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-append-float.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include <strbuf.h>
#include <echeck.h>

#include <assert.h>
#include <string.h>

int test_append_float(const char *in, long double append, const char *expected)
{
	size_t in_len = in ? strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, in_len);

	int failures = 0;

	failures += check_size_t(strbuf_len(sb), in_len);

	failures += check_str(strbuf_append_float(sb, append), expected);

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

	failures += test_append_float("foo: ", 1.2, "foo: 1.2");
	failures += test_append_float("bar: ", -1.0 / 3.0, "bar: -0.333333");
	failures += test_append_float("baz: ", 6000000000.0, "baz: 6e+09");

	char expected[80];
	snprintf(expected, 80, "-LDBL_MIN: %Lg", -LDBL_MIN);
	failures += test_append_float("-LDBL_MIN: ", -LDBL_MIN, expected);

	if (failures) {
		fprintf(stderr, "%d failures in %s\n", failures, __FILE__);
	}

	return failures ? 1 : 0;
}
