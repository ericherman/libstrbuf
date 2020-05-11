/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-prepend.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "../src/strbuf.h"
#include "../submodules/libecheck/src/echeck.h"

#include <assert.h>
#include <string.h>

int test_prepend(const char *prepend, const char *in, const char *expected)
{
	size_t in_len = in ? strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, in_len);

	int failures = 0;

	failures += check_size_t(strbuf_len(sb), in_len);

	size_t alen = prepend ? strlen(prepend) : 0;
	failures += check_str(strbuf_prepend(sb, prepend, alen), expected);
	failures += check_char(strbuf_char(sb, alen), in ? *in : '\0');

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

	failures += test_prepend("foo", "foo", "foofoo");
	failures += test_prepend("bar", NULL, "bar");
	failures += test_prepend("", "baz", "baz");
	failures += test_prepend(NULL, "wiz", "wiz");

	if (failures) {
		fprintf(stderr, "%d failures in %s\n", failures, __FILE__);
	}

	return failures ? 1 : 0;
}
