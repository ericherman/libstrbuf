/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-append-int.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "../src/strbuf.h"
#include "../submodules/libecheck/src/echeck.h"

#include <assert.h>
#include <string.h>

int test_append_int(const char *in, int64_t append, const char *expected)
{
	size_t in_len = in ? strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, in_len);

	int failures = 0;

	failures += check_size_t(strbuf_len(sb), in_len);

	failures += check_str(strbuf_append_int(sb, append), expected);

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

	failures += test_append_int("0: ", 0, "0: 0");
	failures += test_append_int("23: ", 23, "23: 23");
	failures += test_append_int("minus 5: ", -5, "minus 5: -5");

	failures +=
	    test_append_int("INT64_MAX: ", INT64_MAX,
			    "INT64_MAX: 9223372036854775807");
	failures +=
	    test_append_int("INT64_MIN: ", INT64_MIN,
			    "INT64_MIN: -9223372036854775808");

	if (failures) {
		fprintf(stderr, "%d failures in %s\n", failures, __FILE__);
	}

	return failures ? 1 : 0;
}
