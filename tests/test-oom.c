/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-oom.c: tests out of memeory */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include <strbuf.h>
#include <echeck.h>
#include <oom-injecting-malloc.h>

#include <assert.h>
#include <string.h>

int test_out_of_memory_construction(uint64_t allocs_to_fail)
{
	int failures = 0;
	oom_injecting_context_s ctx;

	oom_injecting_context_init(&ctx);
	ctx.attempts_to_fail_bitmask = allocs_to_fail;

	strbuf_s *sb =
	    strbuf_new_custom(oom_injecting_malloc, oom_injecting_free, &ctx,
			      NULL, 0, "", 0);
	if (sb) {
		if (allocs_to_fail) {
			++failures;
		}
		strbuf_destroy(sb);
	} else if (!allocs_to_fail) {
		++failures;
	}

	failures += check_unsigned_int_m(ctx.frees, ctx.allocs, "alloc/free");
	failures +=
	    check_unsigned_int_m(ctx.free_bytes, ctx.alloc_bytes, "bytes");

	return failures;
}

int test_out_of_memory_construction_buf(uint64_t allocs_to_fail)
{
	int failures = 0;
	oom_injecting_context_s ctx;

	oom_injecting_context_init(&ctx);
	ctx.attempts_to_fail_bitmask = allocs_to_fail;

	unsigned char buf[65];

	const char *longstr = "123456789 123456789 123456789 1234567890";
	strbuf_s *sb =
	    strbuf_new_custom(oom_injecting_malloc, oom_injecting_free, &ctx,
			      buf, 65, longstr, strlen(longstr));
	if (sb) {
		if (allocs_to_fail) {
			++failures;
		}
		strbuf_destroy(sb);
	} else if (!allocs_to_fail) {
		++failures;
	}

	failures += check_unsigned_int_m(ctx.frees, ctx.allocs, "alloc/free");
	failures +=
	    check_unsigned_int_m(ctx.free_bytes, ctx.alloc_bytes, "bytes");

	return failures;
}

int main(void)
{
	int failures = 0;

	failures += test_out_of_memory_construction(0);
	failures += test_out_of_memory_construction(1UL << 0);
	failures += test_out_of_memory_construction(1UL << 1);

	failures += test_out_of_memory_construction_buf(0);
	failures += test_out_of_memory_construction_buf(1UL << 0);

	return failures ? 1 : 0;
}
