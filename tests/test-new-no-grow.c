/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-new-no-grow.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "strbuf.h"
#include "echeck.h"

unsigned test_destroy_null(void)
{
	strbuf_s *sb = NULL;
	/* calling destroy on NULL should be okay */
	strbuf_destroy(sb);
	return 0;
}

unsigned test_custom_small_buffer(void)
{
	unsigned failures = 0;
	struct eembed_allocator *orig = eembed_global_allocator;
#if !EEMBED_HOSTED
	const size_t bytes_len = 125 * sizeof(void *);
	unsigned char bytes[125 * sizeof(void *)];
	struct eembed_allocator *ea = eembed_bytes_allocator(bytes, bytes_len);
	eembed_global_allocator = ea;
#endif

	size_t buf_size = 65;
	unsigned char mem_buf[buf_size];
	const char *str = "123456789 1234567890";

	strbuf_s *sb =
	    strbuf_new_custom(NULL, mem_buf, buf_size, str, eembed_strlen(str));

	failures += check_ptr_not_null(sb);
	if (!sb) {
		return 1;
	}

	failures += check_str(strbuf_str(sb), str);

	str = "123456789 123456789 123456789 123456789 1234567890";
	const char *s = strbuf_set(sb, str, eembed_strlen(str));

	failures += check_str(s, str);

	strbuf_destroy(sb);

	eembed_global_allocator = orig;
	return failures;
}

unsigned test_no_grow(void)
{
	unsigned failures = 0;

	size_t buf_size = 125 * sizeof(void *);
	unsigned char buf[buf_size];
	strbuf_s *sb = strbuf_no_grow(buf, buf_size, NULL, 0);
	failures += check_ptr_not_null(sb);
	if (!sb) {
		return failures;
	}
	/* sizeof(strbuf_s) is about 64 */
	/* we expect only about 190 bytes left to make use of a string data */

	strbuf_set(sb, "", 0);
	size_t adds = 0;
	size_t ooms = 0;
	for (size_t i = 0; i < (2 * buf_size); ++i) {
		char c = (i < 10) ? ('a' + i) : ' ';
		size_t buf_size = i + 1;
		char buf[buf_size];
		eembed_memset(buf, c, i);
		buf[buf_size - 1] = '\0';

		const void *p;
		if (i % 2) {
			p = strbuf_prepend(sb, buf, buf_size);
		} else {
			p = strbuf_prepend_f(sb, buf_size, "%s", buf);
		}
		if (!p) {
			++ooms;
		} else {
			++adds;
		}
	}
	failures += check_int(adds > 2 ? 1 : 0, 1);
	failures += check_int(ooms > 0 ? 1 : 0, 1);

	strbuf_trim(sb);

	const void *p = strbuf_prepend(sb, "foo", 3);
	failures += check_int(p ? 1 : 0, 1);

	const size_t bigbuf_len = (150 * sizeof(void *));
	char bigbuf[bigbuf_len + 1];
	bigbuf[bigbuf_len] = '\0';
	eembed_memset(bigbuf, 'x', bigbuf_len);
	p = strbuf_set(sb, bigbuf, bigbuf_len);
	failures += check_int(p ? 1 : 0, 0);

	strbuf_set(sb, "", 0);
	adds = 0;
	ooms = 0;
	for (size_t i = 0; i < (2 * buf_size); ++i) {
		char c = (i < 10) ? ('a' + i) : ' ';
		size_t buf_size = i + 1;
		char buf[buf_size];
		eembed_memset(buf, c, i);
		buf[buf_size - 1] = '\0';

		const void *p;
		if (i % 2) {
			p = strbuf_append(sb, buf, buf_size);
		} else {
			p = strbuf_append_f(sb, buf_size, "%s", buf);
		}
		if (!p) {
			++ooms;
		} else {
			++adds;
		}
	}
	failures += check_int(adds > 10 ? 1 : 0, 1);
	failures += check_int(ooms > 0 ? 1 : 0, 1);

	strbuf_set(sb, "", 0);
	adds = 0;
	ooms = 0;
	for (size_t i = 0; i < (2 * buf_size); ++i) {
		const char *str = (i < 10) ? "x" : " ";
		if (i % 2) {
			p = strbuf_prepend(sb, str, 1);
		} else {
			p = strbuf_prepend_f(sb, 1, "%s", str);
		}
		if (!p) {
			++ooms;
		} else {
			++adds;
		}
	}
	failures += check_int(adds > 2 ? 1 : 0, 1);
	failures += check_int(ooms > 0 ? 1 : 0, 1);

	strbuf_destroy(sb);

	return failures;
}

/* TODO: split into 3 tests? */
unsigned test_new_no_grow(void)
{
	unsigned failures = 0;

	failures += test_destroy_null();
	failures += test_custom_small_buffer();
	failures += test_no_grow();

	return failures;
}

ECHECK_TEST_MAIN(test_new_no_grow)
