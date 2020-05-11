/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-prepend-f.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "../src/strbuf.h"
#include "../submodules/libecheck/src/echeck.h"

#include <assert.h>
#include <string.h>

int test_prepend_f(size_t max, const char *prepend, const char *in,
		   const char *expected)
{
	size_t in_len = in ? strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, in_len);

	int failures = 0;

	failures += check_size_t(strbuf_len(sb), in_len);

	failures +=
	    check_str(strbuf_prepend_f(sb, max, "%s", prepend), expected);
	size_t pos = prepend ? strlen(prepend) : strlen("(null)");
	failures += check_char(strbuf_char(sb, pos), in ? *in : '\0');

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

	failures += test_prepend_f(3, "foo", "foo", "foofoo");
	failures += test_prepend_f(300, "foo", "foo", "foofoo");
	failures += test_prepend_f(3, "bar", NULL, "bar");
	failures += test_prepend_f(3, "", "baz", "baz");
	failures += test_prepend_f(3, NULL, "wiz", "(null)wiz");

	if (failures) {
		fprintf(stderr, "%d failures in %s\n", failures, __FILE__);
	}

	return failures ? 1 : 0;
}
