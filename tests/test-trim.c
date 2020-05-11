/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-trim.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "../src/strbuf.h"
#include "../submodules/libecheck/src/echeck.h"

#include <assert.h>
#include <string.h>

typedef const char *(*trim_func)(strbuf_s *sb);

int test_trim_func(trim_func tfunc, const char *in, const char *expected)
{
	size_t len = in ? strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, len);

	int failures = 0;

	failures += check_size_t(strbuf_len(sb), strlen(in));

	failures += check_str_m(tfunc(sb), expected, in);

	failures += check_size_t(strbuf_len(sb), strlen(expected));

	failures += check_str_m(strbuf_str(sb), expected, in);

	strbuf_destroy(sb);

	return failures;
}

int test_trim(const char *in, const char *expected)
{
	return test_trim_func(strbuf_trim, in, expected);
}

int test_trim_l(const char *in, const char *expected)
{
	return test_trim_func(strbuf_trim_l, in, expected);
}

int test_trim_r(const char *in, const char *expected)
{
	return test_trim_func(strbuf_trim_r, in, expected);
}

int main(int argc, char **argv)
{
	int failures = 0;
	assert(argc);
	assert(argv);

	failures += test_trim("foo", "foo");
	failures += test_trim(" bar ", "bar");
	failures += test_trim("  \tbaz\n", "baz");

	failures += test_trim_l("  whiz\t\n", "whiz\t\n");
	failures += test_trim_r("  bang\t\n", "  bang");

	failures += test_trim("  \t\n", "");
	failures += test_trim_l("  \t\n", "");
	failures += test_trim_r("  \t\n", "");

	if (failures) {
		fprintf(stderr, "%d failures in %s\n", failures, __FILE__);
	}

	return failures ? 1 : 0;
}
