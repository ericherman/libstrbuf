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

#include "eembed.h"

#if EEMBED_HOSTED
#include <stdio.h>
int (*strbuf_vsnprintf)(char *str, size_t size, const char *format, va_list ap)
    = vsnprintf;
#else
int strbuf_no_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
	(void)str;
	(void)size;
	(void)format;
	(void)ap;
	return -1;
}

int (*strbuf_vsnprintf)(char *str, size_t size, const char *format, va_list ap)
    = strbuf_no_vsnprintf;
#endif

struct strbuf {
	char *buf;
	size_t buf_len;
	size_t start;
	size_t end;
	struct eembed_allocator *ea;
	uint8_t flags;
};
typedef struct strbuf strbuf_s;

enum strbuf_flag {
	strbuf_flag_struct_needs_free = 0,
	strbuf_flag_buf_needs_free = 1,
};

static void strbuf_flag_set(strbuf_s *sb, enum strbuf_flag flag, bool val)
{
	uint8_t offset = (uint8_t)flag;
	if (val) {
		sb->flags |= (1U << offset);
	} else {
		sb->flags &= ~(1U << offset);
	}
}

static bool strbuf_flag_get(strbuf_s *sb, enum strbuf_flag flag)
{
	uint8_t offset = (uint8_t)flag;
	return ((sb->flags >> offset) & 0x01) ? true : false;
}

static bool strbuf_buf_needs_free(strbuf_s *sb)
{
	return strbuf_flag_get(sb, strbuf_flag_buf_needs_free);
}

static bool strbuf_struct_needs_free(strbuf_s *sb)
{
	return strbuf_flag_get(sb, strbuf_flag_struct_needs_free);
}

static void strbuf_set_buf_needs_free(strbuf_s *sb, bool val)
{
	strbuf_flag_set(sb, strbuf_flag_buf_needs_free, val);
}

static void strbuf_set_struct_needs_free(strbuf_s *sb, bool val)
{
	strbuf_flag_set(sb, strbuf_flag_struct_needs_free, val);
}

void strbuf_destroy(strbuf_s *sb)
{
	if (!sb) {
		return;
	}
	if (strbuf_buf_needs_free(sb)) {
		struct eembed_allocator *ea = sb->ea;
		ea->free(ea, sb->buf);
		sb->buf = NULL;
	}
	if (strbuf_struct_needs_free(sb)) {
		struct eembed_allocator *ea = sb->ea;
		ea->free(ea, sb);
	}
}

strbuf_s *strbuf_new_custom(struct eembed_allocator *ea,
			    unsigned char *mem_buf, size_t buf_len,
			    const char *str, size_t str_len)
{
	size_t min_initial_size = eembed_align(EEMBED_WORD_LEN > 4 ? 24 : 12);

	if (ea == NULL) {
		ea = eembed_global_allocator;
	}

	strbuf_s *sb = NULL;
	if (mem_buf && (buf_len > (sizeof(strbuf_s)))) {
		eembed_memset(mem_buf, 0x00, buf_len);
		sb = (strbuf_s *)mem_buf;
		strbuf_set_struct_needs_free(sb, false);
		size_t strbuf_size = eembed_align(sizeof(strbuf_s));
		size_t min_len = str_len + 1;
		if (min_len < min_initial_size) {
			min_len = min_initial_size;
		}
		size_t needed = (strbuf_size + min_len);
		if (buf_len > needed) {
			sb->buf = (char *)(mem_buf + strbuf_size);
			sb->buf_len = buf_len - strbuf_size;
			strbuf_set_buf_needs_free(sb, false);
		} else {
			sb->buf = NULL;
			sb->buf_len = 0;
		}
	} else {
		size_t size = sizeof(strbuf_s);
		sb = (strbuf_s *)ea->malloc(ea, size);
		if (!sb) {
			return NULL;
		}
		eembed_memset(sb, 0x00, size);
		strbuf_set_struct_needs_free(sb, true);
	}

	if (sb->buf == NULL) {
		if (!str || !str_len) {
			buf_len = 0;
		} else {
			buf_len = eembed_strnlen(str, str_len);
		}
		buf_len += 1;
		if (buf_len < min_initial_size) {
			buf_len = min_initial_size;
		}

		sb->buf = (char *)ea->malloc(ea, buf_len);
		if (!sb->buf) {
			if (strbuf_struct_needs_free(sb)) {
				ea->free(ea, sb);
			}
			return NULL;
		}
		strbuf_set_buf_needs_free(sb, true);
		sb->buf_len = buf_len;
	}
	sb->ea = ea;

	eembed_memset(sb->buf, 0x00, sb->buf_len);
	sb->start = 0;
	sb->end = 0;

	eembed_assert(str_len < sb->buf_len);
	const char *result = strbuf_set(sb, str, str_len);
	eembed_assert(result);
	(void)result;

	return sb;
}

strbuf_s *strbuf_new(const char *str, size_t str_len)
{
	struct eembed_allocator *ea = NULL;
	unsigned char *initial_buf = NULL;
	size_t initial_buf_len = 0;
	return strbuf_new_custom(ea, initial_buf, initial_buf_len, str,
				 str_len);
}

strbuf_s *strbuf_no_grow(unsigned char *initial_buf, size_t initial_buf_len,
			 const char *str, size_t str_len)
{
	struct eembed_allocator *ea = eembed_null_allocator;
	return strbuf_new_custom(ea, initial_buf, initial_buf_len, str,
				 str_len);
}

const char *strbuf_str(strbuf_s *sb)
{
	eembed_assert(sb);
	const char *str = sb->buf + sb->start;
	return str;
}

size_t strbuf_len(strbuf_s *sb)
{
	eembed_assert(sb);
	size_t str_len = sb->end - sb->start;
	return str_len;
}

size_t strbuf_avail(strbuf_s *sb)
{
	eembed_assert(sb);
	size_t avail = 0;
	if (sb->buf_len) {
		size_t usable_len = sb->buf_len - 1;
		size_t slen = strbuf_len(sb);
		eembed_assert(usable_len >= slen);
		if (usable_len > slen) {
			avail = usable_len - slen;
		}
	}
	return avail;
}

char strbuf_char(strbuf_s *sb, size_t idx)
{
	eembed_assert(sb);
	size_t len = strbuf_len(sb);
	if (idx >= len) {
		return '\0';
	}
	const char *str = strbuf_str(sb);
	char c = str[idx];
	return c;
}

const char *strbuf_rehome(strbuf_s *sb)
{
	eembed_assert(sb);
	if (sb->start) {
		size_t len = sb->end - sb->start;
		const char *str = sb->buf + sb->start;
		void *p = eembed_memmove(sb->buf, str, len);
		eembed_assert(p);
		(void)p;
		sb->start = 0;
		sb->end = len;
		size_t remaining = sb->buf_len - sb->end;
		char *extra = sb->buf + sb->end;
		eembed_memset(extra, 0x00, remaining);
	}
	return strbuf_str(sb);
}

const char *strbuf_grow(strbuf_s *sb, size_t new_buf_len)
{
	eembed_assert(sb);
	if (new_buf_len <= sb->buf_len) {
		return strbuf_rehome(sb);
	}

	new_buf_len = eembed_align(new_buf_len);

	struct eembed_allocator *ea = sb->ea;
	char *new_buf = (char *)ea->malloc(ea, new_buf_len);
	if (!new_buf) {
		return NULL;
	}
	eembed_assert(sb->end >= sb->start);
	size_t str_len = (sb->end - sb->start);
	if (str_len) {
		void *p = eembed_memcpy(new_buf, sb->buf + sb->start, str_len);
		eembed_assert(p);
		(void)p;
	}
	size_t remaining = new_buf_len - str_len;
	eembed_memset(new_buf + str_len, 0x00, remaining);

	if (strbuf_buf_needs_free(sb)) {
		ea->free(ea, sb->buf);
		sb->buf = NULL;
		sb->buf_len = 0;
	}
	sb->buf = new_buf;
	sb->buf_len = new_buf_len;
	strbuf_set_buf_needs_free(sb, true);
	sb->start = 0;
	sb->end = str_len;
	return strbuf_str(sb);
}

const char *strbuf_set(strbuf_s *sb, const char *str, size_t str_len)
{
	eembed_assert(sb);
	if (!str || !str_len) {
		sb->start = 0;
		sb->end = 0;
		eembed_memset(sb->buf, 0x00, sb->buf_len);
		return sb->buf;
	}
	str_len = eembed_strnlen(str, str_len);
	if (str_len >= sb->buf_len) {
		const char *str = strbuf_grow(sb, str_len + 1);
		if (!str) {
			return NULL;
		}
	}
	sb->start = 0;
	eembed_strncpy(sb->buf, str, str_len);
	sb->buf[str_len] = '\0';
	sb->end = eembed_strnlen(sb->buf, sb->buf_len);
	size_t remaining = sb->buf_len - sb->end;
	eembed_memset(sb->buf + sb->end, 0x00, remaining);
	return strbuf_str(sb);
}

const char *strbuf_append(strbuf_s *sb, const char *str, size_t str_max)
{
	eembed_assert(sb);
	size_t str_len;
	if (!str) {
		str = "(null)";
		str_len = 6;
	} else {
		str_len = eembed_strnlen(str, str_max);
	}
	size_t needed = str_len + 1;
	size_t remaining = sb->buf_len - sb->end;
	if (remaining < needed) {
		size_t len = strbuf_len(sb) + needed;
		const char *str = strbuf_grow(sb, len);
		if (!str) {
			return NULL;
		}
	}
	eembed_strncpy(sb->buf + sb->end, str, str_len);
	sb->end += str_len;
	remaining = sb->buf_len - sb->end;
	eembed_memset(sb->buf + sb->end, 0x00, remaining);
	return strbuf_str(sb);
}

const char *strbuf_append_float(strbuf_s *sb, long double ld)
{
	eembed_assert(sb);
	const size_t buf_len = 3 + LDBL_MANT_DIG + (-LDBL_MIN_EXP);
	char buf[buf_len];

	eembed_float_to_str(buf, buf_len, ld);
	return strbuf_append(sb, buf, buf_len);
}

const char *strbuf_append_int(strbuf_s *sb, int64_t i)
{
	eembed_assert(sb);
	const size_t buf_len = 25;
	char buf[25];

	eembed_long_to_str(buf, buf_len, i);
	return strbuf_append(sb, buf, buf_len);
}

const char *strbuf_append_uint(strbuf_s *sb, uint64_t u)
{
	eembed_assert(sb);
	const size_t buf_len = 25;
	char buf[25];

	eembed_ulong_to_str(buf, buf_len, u);
	return strbuf_append(sb, buf, buf_len);
}

static size_t _strbuf_append_f_min_size(size_t max)
{
	if (max < eembed_strnlen("(null)", 6)) {
		max = eembed_strnlen("(null)", 6);
	}
	if (max < SIZE_MAX) {
		++max;		/* room for trailing null */
	}
	return max;
}

const char *strbuf_append_f(strbuf_s *sb, size_t max, const char *format, ...)
{
	eembed_assert(sb);
	size_t buf_len = 25;
	char stack_buf[25];
	char *tmp_buf;
	max = _strbuf_append_f_min_size(max);
	if (max < buf_len) {
		tmp_buf = stack_buf;
	} else {
		struct eembed_allocator *ea = sb->ea;
		tmp_buf = (char *)ea->malloc(ea, max);
		if (!tmp_buf) {
			return NULL;
		}
		buf_len = max;
	}

	va_list args;
	va_start(args, format);
	int printed = strbuf_vsnprintf(tmp_buf, buf_len, format, args);
	va_end(args);

	const char *rv = NULL;
	if (printed >= 0) {
		rv = strbuf_append(sb, tmp_buf, buf_len);
	}

	if (tmp_buf != stack_buf) {
		struct eembed_allocator *ea = sb->ea;
		ea->free(ea, tmp_buf);
	}

	return rv;
}

const char *strbuf_prepend(strbuf_s *sb, const char *str, size_t str_len)
{
	eembed_assert(sb);
	size_t add_len = eembed_strnlen(str, str_len);
	size_t old_len = strbuf_len(sb);
	size_t unused = strbuf_avail(sb);
	const void *p;
	if (unused <= add_len) {
		p = strbuf_grow(sb, strbuf_len(sb) + add_len + 1);
		if (!p) {
			return NULL;
		}
	}
	p = eembed_memmove(sb->buf + add_len, sb->buf + sb->start, old_len);
	eembed_assert(p);
	sb->start = add_len;
	sb->end = add_len + old_len;
	size_t remaining = sb->buf_len - sb->end;
	eembed_memset(sb->buf + sb->end, 0x00, remaining);
	p = eembed_memmove(sb->buf, str, add_len);
	eembed_assert(p);
	sb->start = 0;
	return strbuf_str(sb);
}

const char *strbuf_prepend_float(strbuf_s *sb, long double ld)
{
	eembed_assert(sb);
	const size_t buf_len = 1 + 3 + LDBL_MANT_DIG + (-LDBL_MIN_EXP);
	char buf[buf_len];

	eembed_float_to_str(buf, buf_len, ld);
	return strbuf_prepend(sb, buf, buf_len);
}

const char *strbuf_prepend_int(strbuf_s *sb, int64_t i)
{
	eembed_assert(sb);
	const size_t buf_len = 1 + 20;
	char buf[buf_len];

	eembed_long_to_str(buf, buf_len, i);
	return strbuf_prepend(sb, buf, buf_len);
}

const char *strbuf_prepend_uint(strbuf_s *sb, uint64_t u)
{
	eembed_assert(sb);
	const size_t buf_len = 1 + 20;
	char buf[buf_len];

	eembed_ulong_to_str(buf, buf_len, u);
	return strbuf_prepend(sb, buf, buf_len);
}

const char *strbuf_prepend_f(strbuf_s *sb, size_t max, const char *format, ...)
{
	eembed_assert(sb);
	size_t buf_len = 25;
	char stack_buf[25];
	char *tmp_buf;
	max = _strbuf_append_f_min_size(max);
	if (max < buf_len) {
		tmp_buf = stack_buf;
	} else {
		struct eembed_allocator *ea = sb->ea;
		tmp_buf = (char *)ea->malloc(ea, max);
		if (!tmp_buf) {
			return NULL;
		}
		buf_len = max;
	}

	va_list args;
	va_start(args, format);
	int printed = strbuf_vsnprintf(tmp_buf, buf_len, format, args);
	va_end(args);

	const char *rv = NULL;
	if (printed >= 0) {
		rv = strbuf_prepend(sb, tmp_buf, buf_len);
	}

	if (tmp_buf != stack_buf) {
		struct eembed_allocator *ea = sb->ea;
		ea->free(ea, tmp_buf);
	}

	return rv;
}

static int strbuf_isspace(char c)
{
	switch (c) {
	case '\t':
	case '\n':
	case '\v':
	case '\f':
	case '\r':
	case ' ':
		return 1;
	default:
		return 0;
	}
}

const char *strbuf_trim_l(strbuf_s *sb)
{
	eembed_assert(sb);
	while ((sb->start < sb->end) && strbuf_isspace(sb->buf[sb->start])) {
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
	eembed_assert(sb);
	while ((sb->start < sb->end) && strbuf_isspace(sb->buf[sb->end - 1])) {
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
	eembed_assert(sb);
	strbuf_trim_l(sb);
	strbuf_trim_r(sb);
	return strbuf_str(sb);
}

char *strbuf_expose(strbuf_s *sb, size_t *size)
{
	eembed_assert(sb);
	eembed_assert(size);

	strbuf_rehome(sb);

	*size = sb->buf_len;
	return sb->buf;
}

const char *strbuf_return(strbuf_s *sb)
{
	eembed_assert(sb);
	eembed_assert(sb->start == 0);
	sb->end = eembed_strnlen(sb->buf, sb->buf_len);
	return strbuf_str(sb);
}
