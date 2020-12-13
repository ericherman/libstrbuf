/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-trim.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "strbuf.h"
#include "echeck.h"

typedef const char *(*trim_func)(strbuf_s *sb);

unsigned test_trim_func(trim_func tfunc, const char *in, const char *expected)
{
	unsigned failures = 0;
	struct eembed_allocator *orig = eembed_global_allocator;
#if !EEMBED_HOSTED
	const size_t bytes_len = 125 * sizeof(void *);
	unsigned char bytes[125 * sizeof(void *)];
	struct eembed_allocator *ea = eembed_bytes_allocator(bytes, bytes_len);
	eembed_global_allocator = ea;
#endif

	size_t len = in ? eembed_strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, len);

	failures += check_size_t(strbuf_len(sb), eembed_strlen(in));

	failures += check_str_m(tfunc(sb), expected, in);

	failures += check_size_t(strbuf_len(sb), eembed_strlen(expected));

	failures += check_str_m(strbuf_str(sb), expected, in);

	strbuf_destroy(sb);

	eembed_global_allocator = orig;
	return failures;
}

unsigned test_trim_lr(const char *in, const char *expected)
{
	return test_trim_func(strbuf_trim, in, expected);
}

unsigned test_trim_l(const char *in, const char *expected)
{
	return test_trim_func(strbuf_trim_l, in, expected);
}

unsigned test_trim_r(const char *in, const char *expected)
{
	return test_trim_func(strbuf_trim_r, in, expected);
}

unsigned test_trim(void)
{
	unsigned failures = 0;

	failures += test_trim_lr("foo", "foo");
	failures += test_trim_lr(" bar ", "bar");
	failures += test_trim_lr("  \tbaz\n", "baz");

	failures += test_trim_l("  whiz\t\n", "whiz\t\n");
	failures += test_trim_r("  bang\t\n", "  bang");

	failures += test_trim_lr("  \t\n", "");
	failures += test_trim_l("  \t\n", "");
	failures += test_trim_r("  \t\n", "");

	return failures;
}

ECHECK_TEST_MAIN(test_trim)
