/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* strbuf.c : string buffer */
/* Copyright (C) 2013, 2020 Eric Herman <eric@freesa.org> */

#include "strbuf.h"

/* freestanding headers */
#include <float.h>
/* #include <iso646.h> */
#include <limits.h>
#include <stdalign.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
/* #include <stdnoreturn.h> */

/* hosted headers */
#ifndef assert
#include <assert.h>
#endif

#ifndef Sb_malloc
#include <stdlib.h>
#define Sb_malloc(size) malloc(size)
#define Sb_free(p) free(p)
#endif

#ifndef Sb_isspace
#include <ctype.h>
#define Sb_isspace(c) isspace(c)
#endif

#ifndef Sb_snprintf
#include <stdio.h>
#define Sb_snprintf(buf, len, ...) snprintf(buf, len, __VA_ARGS__)
#endif

#ifndef Sb_vsnprintf
#include <stdio.h>
#define Sb_vsnprintf(buf, len, ...) vsnprintf(buf, len, __VA_ARGS__)
#endif

#ifndef Sb_memcpy
#include <string.h>
#define Sb_memcpy(dest, src, n) memcpy(dest, src, n)
#endif

#ifndef Sb_memmove
#include <string.h>
#define Sb_memmove(dest, src, n) memmove(dest, src, n)
#endif

#ifndef Sb_memset
#include <string.h>
#define Sb_memset(dest, val, n) memset(dest, val, n)
#endif

#ifndef Sb_strncpy
#include <string.h>
#define Sb_strncpy(dest, src, n) strncpy(dest, src, n)
#endif

#ifndef Sb_strnlen
#include <string.h>
#define Sb_strnlen(str, max) strnlen(str, max)
#endif

#ifndef Sb_imaxabs
#include <inttypes.h>
#define Sb_imaxabs(j) imaxabs(j)
#endif

#define Sb_ptrsize (sizeof(char *))
#define Sb_align(x) \
        (((x) + ((Sb_ptrsize) - 1)) \
             & ~((Sb_ptrsize) - 1))

#ifndef Sb_min_initial_size
#define Sb_min_initial_size Sb_ptrsize
#endif

#ifndef Sb_stack_buf_len
#define Sb_stack_buf_len Sb_min_initial_size
#endif

struct strbuf {
	char *buf;
	size_t buf_len;
	size_t start;
	size_t end;
	strbuf_malloc_func sb_alloc;
	strbuf_free_func sb_free;
	void *alloc_context;
	bool struct_needs_free;
	bool buf_needs_free;
};
typedef struct strbuf strbuf_s;

void *strbuf_malloc(void *context, size_t size);
void strbuf_free(void *context, void *ptr);
void *strbuf_fail_alloc(void *context, size_t size);

void strbuf_destroy(strbuf_s *sb)
{
	if (!sb) {
		return;
	}
	if (sb->buf_needs_free) {
		sb->sb_free(sb->alloc_context, sb->buf);
		sb->buf = NULL;
	}
	if (sb->struct_needs_free) {
		sb->sb_free(sb->alloc_context, sb);
	}
}

strbuf_s *strbuf_new_custom(strbuf_malloc_func alloc_func,
			    strbuf_free_func free_func,
			    void *alloc_context,
			    unsigned char *mem_buf, size_t buf_len,
			    const char *str, size_t str_len)
{
	if (alloc_func == NULL || free_func == NULL) {
		alloc_func = strbuf_malloc;
		free_func = strbuf_free;
		alloc_context = NULL;
	}

	strbuf_s *sb = NULL;
	if (mem_buf && (buf_len > (sizeof(strbuf_s)))) {
		Sb_memset(mem_buf, 0x00, buf_len);
		sb = (strbuf_s *)mem_buf;
		sb->struct_needs_free = 0;
		size_t strbuf_size = Sb_align(sizeof(strbuf_s));
		if (buf_len > (strbuf_size + Sb_min_initial_size)) {
			sb->buf = (char *)(mem_buf + strbuf_size);
			sb->buf_len = buf_len - strbuf_size;
			sb->buf_needs_free = 0;
		} else {
			sb->buf = NULL;
			sb->buf_len = 0;
		}
	} else {
		size_t size = sizeof(strbuf_s);
		sb = alloc_func(alloc_context, size);
		if (!sb) {
			return NULL;
		}
		Sb_memset(sb, 0x00, size);
		sb->struct_needs_free = 1;
	}

	if (sb->buf == NULL) {
		if (!str || !str_len) {
			buf_len = 0;
		} else {
			buf_len = Sb_strnlen(str, str_len);
		}
		buf_len += 1;
		if (buf_len < Sb_min_initial_size) {
			buf_len = Sb_min_initial_size;
		}

		sb->buf = alloc_func(alloc_context, buf_len);
		if (!sb->buf) {
			if (sb->struct_needs_free) {
				free_func(alloc_context, sb);
			}
			return NULL;
		}
		sb->buf_needs_free = 1;
		sb->buf_len = buf_len;
	}
	sb->sb_alloc = alloc_func;
	sb->sb_free = free_func;
	sb->alloc_context = alloc_context;

	Sb_memset(sb->buf, 0x00, sb->buf_len);
	sb->start = 0;
	sb->end = 0;

	assert(str_len < sb->buf_len);
	const char *result = strbuf_set(sb, str, str_len);
	assert(result);
	return sb;
}

strbuf_s *strbuf_new(const char *str, size_t str_len)
{
	void *alloc_context = NULL;
	unsigned char *initial_buf = NULL;
	size_t initial_buf_len = 0;
	return strbuf_new_custom(strbuf_malloc, strbuf_free, alloc_context,
				 initial_buf, initial_buf_len, str, str_len);
}

strbuf_s *strbuf_no_grow(unsigned char *initial_buf, size_t initial_buf_len,
			 const char *str, size_t str_len)
{
	void *alloc_context = NULL;
	return strbuf_new_custom(strbuf_fail_alloc, strbuf_free, alloc_context,
				 initial_buf, initial_buf_len, str, str_len);
}

const char *strbuf_str(strbuf_s *sb)
{
	return sb->buf + sb->start;
}

size_t strbuf_len(strbuf_s *sb)
{
	return sb->end - sb->start;
}

size_t strbuf_avail(strbuf_s *sb)
{
	size_t usable_len = sb->buf_len ? (sb->buf_len - 1) : 0;
	size_t slen = strbuf_len(sb);
	return usable_len > slen ? usable_len - slen : 0;
}

char strbuf_char(strbuf_s *sb, size_t idx)
{
	if (idx >= strbuf_len(sb)) {
		return '\0';
	}
	return strbuf_str(sb)[idx];
}

const char *strbuf_rehome(strbuf_s *sb)
{
	if (sb->start) {
		size_t len = sb->end - sb->start;
		void *p = Sb_memmove(sb->buf, sb->buf + sb->start, len);
		assert(p);
		sb->start = 0;
		sb->end = len;
		size_t remaining = sb->buf_len - sb->end;
		Sb_memset(sb->buf + sb->end, 0x00, remaining);
	}
	return strbuf_str(sb);
}

const char *strbuf_grow(strbuf_s *sb, size_t new_buf_len)
{
	if (new_buf_len <= sb->buf_len) {
		return strbuf_rehome(sb);
	}

	new_buf_len = Sb_align(new_buf_len);

	char *new_buf = sb->sb_alloc(sb->alloc_context, new_buf_len);
	if (!new_buf) {
		return NULL;
	}
	assert(sb->end >= sb->start);
	size_t str_len = (sb->end - sb->start);
	if (str_len) {
		void *p = Sb_memcpy(new_buf, sb->buf + sb->start, str_len);
		assert(p);
	}
	size_t remaining = new_buf_len - str_len;
	Sb_memset(new_buf + str_len, 0x00, remaining);

	if (sb->buf_needs_free) {
		sb->sb_free(sb->alloc_context, sb->buf);
		sb->buf = NULL;
		sb->buf_len = 0;
	}
	sb->buf = new_buf;
	sb->buf_len = new_buf_len;
	sb->buf_needs_free = 1;
	sb->start = 0;
	sb->end = str_len;
	return strbuf_str(sb);
}

const char *strbuf_set(strbuf_s *sb, const char *str, size_t str_len)
{
	if (!str || !str_len) {
		sb->start = 0;
		sb->end = 0;
		Sb_memset(sb->buf, 0x00, sb->buf_len);
		return sb->buf;
	}
	str_len = Sb_strnlen(str, str_len);
	if (str_len >= sb->buf_len) {
		const char *str = strbuf_grow(sb, str_len + 1);
		if (!str) {
			return NULL;
		}
	}
	sb->start = 0;
	Sb_strncpy(sb->buf, str, str_len);
	sb->buf[str_len] = '\0';
	sb->end = Sb_strnlen(sb->buf, sb->buf_len);
	size_t remaining = sb->buf_len - sb->end;
	Sb_memset(sb->buf + sb->end, 0x00, remaining);
	return strbuf_str(sb);
}

static size_t _strbuf_append_f_min_size(size_t max)
{
	if (max < Sb_strnlen("(null)", 6)) {
		max = Sb_strnlen("(null)", 6);
	}
	if (max < SIZE_MAX) {
		++max;		/* room for trailing null */
	}
	return max;
}

const char *strbuf_append_f(strbuf_s *sb, size_t max, const char *format, ...)
{
	max = _strbuf_append_f_min_size(max);
	size_t remaining = sb->buf_len - sb->end;
	if (remaining < max) {
		const char *str = strbuf_grow(sb, strbuf_len(sb) + max);
		if (!str) {
			return NULL;
		}
	}
	va_list args;
	va_start(args, format);
	int printed = Sb_vsnprintf(sb->buf + sb->end, max, format, args);
	va_end(args);
	assert(printed >= 0);
	sb->end += printed;
	return strbuf_str(sb);
}

const char *strbuf_append(strbuf_s *sb, const char *str, size_t str_len)
{
	size_t add_len = (!str) ? 6 : Sb_strnlen(str, str_len);
	const char *format = "%s";
	return strbuf_append_f(sb, add_len, format, str);
}

const char *strbuf_append_float(strbuf_s *sb, long double ld)
{
	size_t max_add_len = 3 + LDBL_MANT_DIG + (-LDBL_MIN_EXP);
	const char *format = "%Lg";
	return strbuf_append_f(sb, max_add_len, format, ld);
}

const char *strbuf_append_int(strbuf_s *sb, int64_t i)
{
	size_t max_add_len = 20;
	const char *format = "%" PRIi64;
	return strbuf_append_f(sb, max_add_len, format, i);
}

const char *strbuf_append_uint(strbuf_s *sb, uint64_t u)
{
	size_t max_add_len = 20;
	const char *format = "%" PRIu64;
	return strbuf_append_f(sb, max_add_len, format, u);
}

const char *strbuf_prepend(strbuf_s *sb, const char *str, size_t str_len)
{
	size_t add_len = Sb_strnlen(str, str_len);
	size_t old_len = strbuf_len(sb);
	size_t unused = strbuf_avail(sb);
	const void *p;
	if (unused < add_len) {
		p = strbuf_grow(sb, strbuf_len(sb) + add_len);
		if (!p) {
			return NULL;
		}
	}
	p = Sb_memmove(sb->buf + add_len, sb->buf + sb->start, old_len);
	assert(p);
	sb->start = add_len;
	sb->end = add_len + old_len;
	size_t remaining = sb->buf_len - sb->end;
	Sb_memset(sb->buf + sb->end, 0x00, remaining);
	p = Sb_memmove(sb->buf, str, add_len);
	assert(p);
	sb->start = 0;
	return strbuf_str(sb);
}

const char *strbuf_prepend_f(strbuf_s *sb, size_t max, const char *format, ...)
{
	char stack_buf[Sb_stack_buf_len];
	char *tmp_buf;
	max = _strbuf_append_f_min_size(max);
	if (max < Sb_stack_buf_len) {
		tmp_buf = stack_buf;
	} else {
		tmp_buf = sb->sb_alloc(sb->alloc_context, max);
		if (!tmp_buf) {
			return NULL;
		}
	}

	va_list args;
	va_start(args, format);
	int printed = Sb_vsnprintf(tmp_buf, max, format, args);
	va_end(args);
	assert(printed >= 0);

	const char *rv = strbuf_prepend(sb, tmp_buf, max);
	if (tmp_buf != stack_buf) {
		sb->sb_free(sb->alloc_context, tmp_buf);
	}

	return rv;
}

const char *strbuf_prepend_float(strbuf_s *sb, long double ld)
{
	size_t buf_len = 1 + 3 + LDBL_MANT_DIG + (-LDBL_MIN_EXP);
	char buf[buf_len];
	const char *format = "%Lg";
	int printed = Sb_snprintf(buf, buf_len, format, ld);
	assert(printed > 0);
	return strbuf_prepend(sb, buf, buf_len);
}

const char *strbuf_prepend_int(strbuf_s *sb, int64_t i)
{
	size_t buf_len = 1 + 20;
	char buf[buf_len];
	const char *format = "%" PRIi64;
	int printed = Sb_snprintf(buf, buf_len, format, i);
	assert(printed > 0);
	return strbuf_prepend(sb, buf, buf_len);
}

const char *strbuf_prepend_uint(strbuf_s *sb, uint64_t u)
{
	size_t buf_len = 1 + 20;
	char buf[buf_len];
	const char *format = "%" PRIu64;
	int printed = Sb_snprintf(buf, buf_len, format, u);
	assert(printed > 0);
	return strbuf_prepend(sb, buf, buf_len);
}

const char *strbuf_trim_l(strbuf_s *sb)
{
	while ((sb->start < sb->end) && Sb_isspace(sb->buf[sb->start])) {
		sb->buf[sb->start] = '\0';
		++(sb->start);
	}

	if (sb->start == sb->end) {
		sb->start = 0;
		sb->end = 0;
		sb->buf[0] = '\0';
	}

	return strbuf_str(sb);
}

const char *strbuf_trim_r(strbuf_s *sb)
{
	while ((sb->start < sb->end) && Sb_isspace(sb->buf[sb->end - 1])) {
		sb->buf[sb->end - 1] = '\0';
		--(sb->end);
	}

	if (sb->start == sb->end) {
		sb->start = 0;
		sb->end = 0;
		sb->buf[0] = '\0';
	}

	return strbuf_str(sb);
}

const char *strbuf_trim(strbuf_s *sb)
{
	strbuf_trim_l(sb);
	strbuf_trim_r(sb);
	return strbuf_str(sb);
}

void *strbuf_malloc(void *context, size_t size)
{
	assert(context == NULL);
	return Sb_malloc(size);
}

void strbuf_free(void *context, void *ptr)
{
	assert(context == NULL);
	Sb_free(ptr);
}

void *strbuf_fail_alloc(void *context, size_t size)
{
	assert(context == NULL);
	assert(size > 0);
	return NULL;
}
