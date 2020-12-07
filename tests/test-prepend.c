/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-prepend.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include <strbuf.h>
#include <echeck.h>

#include <assert.h>
#include <string.h>

unsigned test_prepend_inner(const char *prepend, const char *in,
			    const char *expected)
{
	unsigned failures = 0;

	size_t in_len = in ? strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, in_len);

	failures += check_size_t(strbuf_len(sb), in_len);

	size_t alen = prepend ? strlen(prepend) : 0;
	failures += check_str(strbuf_prepend(sb, prepend, alen), expected);
	failures += check_char(strbuf_char(sb, alen), in ? *in : '\0');

	failures += check_size_t(strbuf_len(sb), strlen(expected));

	failures += check_str(strbuf_str(sb), expected);

	strbuf_destroy(sb);

	return failures;
}

unsigned test_prepend(void)
{
	unsigned failures = 0;

	failures += test_prepend_inner("foo", "foo", "foofoo");
	failures += test_prepend_inner("bar", NULL, "bar");
	failures += test_prepend_inner("", "baz", "baz");
	failures += test_prepend_inner(NULL, "wiz", "wiz");

	return failures;
}

ECHECK_TEST_MAIN(test_prepend)
