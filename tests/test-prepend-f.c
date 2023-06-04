/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-prepend-f.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "strbuf.h"
#include "echeck.h"

unsigned test_prepend_f_inner(size_t max, const char *prepend, const char *in,
			      const char *expected)
{
	unsigned failures = 0;
	struct eembed_allocator *orig = eembed_global_allocator;
#if !EEMBED_HOSTED
	const size_t bytes_len = 125 * sizeof(void *);
	unsigned char bytes[125 * sizeof(void *)];
	struct eembed_allocator *ea = eembed_bytes_allocator(bytes, bytes_len);
	eembed_global_allocator = ea;
#endif

	size_t in_len = in ? eembed_strlen(in) : 0;

	strbuf_s *sb = strbuf_new(in, in_len);

	failures += check_size_t(strbuf_len(sb), in_len);

	failures +=
	    check_str(strbuf_prepend_f(sb, max, "%s", prepend), expected);
	size_t pos = prepend ? eembed_strlen(prepend) : eembed_strlen("(null)");
	failures += check_char(strbuf_char(sb, pos), in ? *in : '\0');

	failures += check_size_t(strbuf_len(sb), eembed_strlen(expected));

	failures += check_str(strbuf_str(sb), expected);

	strbuf_destroy(sb);

	eembed_global_allocator = orig;
	return failures;
}

unsigned test_prepend_f(void)
{
	unsigned failures = 0;

	if (!EEMBED_HOSTED) {
		struct eembed_log *log = eembed_out_log;
		log->append_s(log, " (skipping test_prepend_f)");
		log->append_eol(log);
		return 0;
	}

	failures += test_prepend_f_inner(3, "foo", "foo", "foofoo");
	failures += test_prepend_f_inner(300, "foo", "foo", "foofoo");
	failures += test_prepend_f_inner(3, "bar", NULL, "bar");
	failures += test_prepend_f_inner(3, "", "baz", "baz");
	failures += test_prepend_f_inner(3, NULL, "wiz", "(null)wiz");

	return failures;
}

ECHECK_TEST_MAIN(test_prepend_f)
