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

	size_t buf_len = 65;
	unsigned char mem_buf[buf_len];
	const char *str = "123456789 1234567890";

	strbuf_s *sb =
	    strbuf_new_custom(NULL, mem_buf, buf_len, str, eembed_strlen(str));

	if (!sb) {
		return 1;
	}

	failures += check_str(strbuf_str(sb), str);

	str = "123456789 123456789 123456789 123456789 1234567890";
	const char *s = strbuf_set(sb, str, eembed_strlen(str));

	failures += check_str(s, str);

	strbuf_destroy(sb);

	return failures;
}

unsigned test_no_grow(void)
{
	unsigned failures = 0;

	size_t buf_len = 255;
	unsigned char buf[buf_len];
	strbuf_s *sb = strbuf_no_grow(buf, buf_len, NULL, 0);
	/* sizeof(strbuf_s) is about 64 */
	/* we expect only about 190 bytes left to make use of a string data */

	strbuf_set(sb, "", 0);
	size_t adds = 0;
	size_t ooms = 0;
	for (size_t i = 0; i < (2 * buf_len); ++i) {
		char c = (i < 10) ? ('a' + i) : ' ';
		size_t buf_len = i + 1;
		char buf[buf_len];
		eembed_memset(buf, c, i);
		buf[buf_len - 1] = '\0';

		const void *p;
		if (i % 2) {
			p = strbuf_prepend(sb, buf, buf_len);
		} else {
			p = strbuf_prepend_f(sb, buf_len, "%s", buf);
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

	const void *p = strbuf_prepend_f(sb, 3, "%s", "foo");
	failures += check_int(p ? 1 : 0, 1);

	char *bigbuf = eembed_calloc(1, 1000);
	eembed_assert(bigbuf);
	eembed_memset(bigbuf, 'x', 999);
	p = strbuf_set(sb, bigbuf, 1000);
	failures += check_int(p ? 1 : 0, 0);
	eembed_free(bigbuf);

	strbuf_set(sb, "", 0);
	adds = 0;
	ooms = 0;
	for (size_t i = 0; i < (2 * buf_len); ++i) {
		char c = (i < 10) ? ('a' + i) : ' ';
		size_t buf_len = i + 1;
		char buf[buf_len];
		eembed_memset(buf, c, i);
		buf[buf_len - 1] = '\0';

		const void *p;
		if (i % 2) {
			p = strbuf_append(sb, buf, buf_len);
		} else {
			p = strbuf_append_f(sb, buf_len, "%s", buf);
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
	for (size_t i = 0; i < (2 * buf_len); ++i) {
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
