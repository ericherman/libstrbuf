/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-append-f.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include <strbuf.h>
#include <echeck.h>

#include <assert.h>
#include <string.h>

int test_append_f(const char *in, const char *append, const char *expected)
{
	size_t in_len = in ? strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, in_len);

	int failures = 0;

	failures += check_size_t(strbuf_len(sb), in_len);

	size_t max = (append ? strlen(append) : 0);
	failures += check_str(strbuf_append_f(sb, max, "%s", append), expected);

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

	failures += test_append_f("foo", "foo", "foofoo");
	failures += test_append_f("bar", NULL, "bar(null)");
	failures += test_append_f("", "baz", "baz");
	failures += test_append_f(NULL, "wiz", "wiz");

	if (failures) {
		fprintf(stderr, "%d failures in %s\n", failures, __FILE__);
	}

	return failures ? 1 : 0;
}
