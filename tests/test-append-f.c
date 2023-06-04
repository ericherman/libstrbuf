/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-append-f.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "strbuf.h"
#include "echeck.h"

unsigned test_append_f_inner(const char *in, const char *append,
			     const char *expected)
{
	unsigned failures = 0;

	size_t in_len = in ? eembed_strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, in_len);

	failures += check_size_t(strbuf_len(sb), in_len);

	size_t max = (append ? eembed_strlen(append) : 0);
	failures += check_str(strbuf_append_f(sb, max, "%s", append), expected);

	failures += check_size_t(strbuf_len(sb), eembed_strlen(expected));

	failures += check_str(strbuf_str(sb), expected);

	strbuf_destroy(sb);

	return failures;
}

unsigned test_append_f(void)
{
	unsigned failures = 0;

	if (!EEMBED_HOSTED) {
		struct eembed_log *log = eembed_out_log;
		log->append_s(log, " (skipping test_append_f)");
		log->append_eol(log);
		return 0;
	}

	failures += test_append_f_inner("foo", "foo", "foofoo");
	failures += test_append_f_inner("bar", NULL, "bar(null)");
	failures += test_append_f_inner("", "baz", "baz");
	failures += test_append_f_inner(NULL, "wiz", "wiz");

	const char *longstr = "_12456789_12456789_12456789";
	const char *lstr_expect = "foo_12456789_12456789_12456789";
	failures += test_append_f_inner("foo", longstr, lstr_expect);

	return failures;
}

ECHECK_TEST_MAIN(test_append_f)
