/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-new-no-grow.c */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include <strbuf.h>
#include <echeck.h>

#include <assert.h>
#include <string.h>

int test_destroy_null(void)
{
	strbuf_s *sb = NULL;
	/* calling destroy on NULL should be okay */
	strbuf_destroy(sb);
	return 0;
}

int test_custom_small_buffer(void)
{
	int failures = 0;

	strbuf_malloc_func alloc_func = NULL;
	strbuf_free_func free_func = NULL;
	void *alloc_context = NULL;
	size_t buf_len = 65;
	unsigned char mem_buf[buf_len];
	const char *str = "123456789 1234567890";

	strbuf_s *sb =
	    strbuf_new_custom(alloc_func, free_func, alloc_context, mem_buf,
			      buf_len, str, strlen(str));

	if (!sb) {
		return 1;
	}

	failures += check_str(strbuf_str(sb), str);

	str = "123456789 123456789 123456789 123456789 1234567890";
	const char *s = strbuf_set(sb, str, strlen(str));

	failures += check_str(s, str);

	strbuf_destroy(sb);

	return failures;
}

int test_no_grow(void)
{
	int failures = 0;

	size_t buf_len = 255;
	unsigned char buf[buf_len];
	strbuf_s *sb = strbuf_no_grow(buf, buf_len, NULL, 0);
	/* sizeof(strbuf_s) is about 64 */
	/* we expect only about 190 bytes left to make use of a string data */

	strbuf_set(sb, "", 0);
	size_t adds = 0;
	size_t ooms = 0;
	for (size_t i = 0; i < (2 * buf_len); ++i) {
		size_t max = 1;
		char c = (i < 10) ? 'x' : ' ';
		const void *p = strbuf_prepend_f(sb, max, "%c", c);
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

	char *bigbuf = calloc(1, 1000);
	assert(bigbuf);
	memset(bigbuf, 'x', 999);
	p = strbuf_set(sb, bigbuf, 1000);
	failures += check_int(p ? 1 : 0, 0);
	free(bigbuf);

	strbuf_set(sb, "", 0);
	adds = 0;
	ooms = 0;
	for (size_t i = 0; i < (2 * buf_len); ++i) {
		size_t max = 1;
		char c = (i < 10) ? 'x' : ' ';
		const void *p = strbuf_append_f(sb, max, "%c", c);
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
		size_t max = i;
		char c = (i < 10) ? 'x' : ' ';
		const void *p = strbuf_prepend_f(sb, max, "%c", c);
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

int main(int argc, char **argv)
{
	int failures = 0;
	assert(argc);
	assert(argv);

	failures += test_destroy_null();
	failures += test_custom_small_buffer();
	failures += test_no_grow();

	if (failures) {
		fprintf(stderr, "%d failures in %s\n", failures, __FILE__);
	}

	return failures ? 1 : 0;
}
