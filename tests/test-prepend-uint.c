/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-prepend-uint.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include <strbuf.h>
#include <echeck.h>

#include <assert.h>
#include <string.h>

int test_prepend_uint(const char *in, uint64_t prepend, const char *expected)
{
	size_t in_len = in ? strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, in_len);

	int failures = 0;

	failures += check_size_t(strbuf_len(sb), in_len);

	failures += check_str(strbuf_prepend_uint(sb, prepend), expected);

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

	failures += test_prepend_uint(" (0)", 0, "0 (0)");
	failures += test_prepend_uint(" (23)", 23, "23 (23)");

	failures +=
	    test_prepend_uint(" UINT64_MAX", UINT64_MAX,
			      "18446744073709551615 UINT64_MAX");
	failures += test_prepend_uint(" (-1)", -1, "18446744073709551615 (-1)");

	if (failures) {
		fprintf(stderr, "%d failures in %s\n", failures, __FILE__);
	}

	return failures ? 1 : 0;
}
