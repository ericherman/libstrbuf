/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* strbuf.h : string buffer headers */
/* Copyright (C) 2103, 2020 Eric Herman <eric@freesa.org> */

#ifndef STRBUF_H
#define STRBUF_H 1

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

struct strbuf;
typedef struct strbuf strbuf_s;

typedef void *(*strbuf_malloc_func)(void *context, size_t size);
typedef void (*strbuf_free_func)(void *context, void *ptr);

strbuf_s *strbuf_new_custom(strbuf_malloc_func alloc_func,
			    strbuf_free_func free_func,
			    void *alloc_context,
			    unsigned char *mem_buf, size_t buf_len,
			    const char *str, size_t str_len);

strbuf_s *strbuf_new(const char *str, size_t strlen);

strbuf_s *strbuf_no_grow(unsigned char *initial_buf, size_t initial_buf_len,
			 const char *str, size_t str_len);

void strbuf_destroy(strbuf_s *sb);

const char *strbuf_str(strbuf_s *sb);

const char *strbuf_set(strbuf_s *sb, const char *str, size_t str_len);

size_t strbuf_len(strbuf_s *sb);

char strbuf_char(strbuf_s *sb, size_t idx);

const char *strbuf_append(strbuf_s *sb, const char *str, size_t len);
const char *strbuf_append_f(strbuf_s *sb, size_t max, const char *format, ...);
const char *strbuf_append_float(strbuf_s *sb, long double f);
const char *strbuf_append_int(strbuf_s *sb, int64_t i);
const char *strbuf_append_uint(strbuf_s *sb, uint64_t u);

const char *strbuf_prepend(strbuf_s *sb, const char *str, size_t len);
const char *strbuf_prepend_f(strbuf_s *sb, size_t max, const char *format, ...);
const char *strbuf_prepend_float(strbuf_s *sb, long double f);
const char *strbuf_prepend_int(strbuf_s *sb, int64_t i);
const char *strbuf_prepend_uint(strbuf_s *sb, uint64_t u);

const char *strbuf_trim(strbuf_s *sb);
const char *strbuf_trim_l(strbuf_s *sb);
const char *strbuf_trim_r(strbuf_s *sb);

#endif /* #ifndef STRBUF_H */
