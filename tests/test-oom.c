/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* test-oom.c: tests out of memeory */
/* Copyright (C) 2020 Eric Herman <eric@freesa.org> */

#include "strbuf.h"
#include "echeck.h"

unsigned test_out_of_memory_construction(const char *msg,
					 unsigned long allocs_to_fail)
{
	unsigned failures = 0;
	struct eembed_allocator *orig = eembed_global_allocator;
	struct eembed_allocator ea;
	struct echeck_err_injecting_context ctx;

	echeck_err_injecting_allocator_init(&ea, orig, &ctx, eembed_err_log);
	ctx.attempts_to_fail_bitmask = allocs_to_fail;

	eembed_global_allocator = &ea;

	strbuf_s *sb = strbuf_new_custom(&ea, NULL, 0, "", 0);
	if (sb) {
		failures += check_unsigned_long_m(allocs_to_fail, 0, msg);
		strbuf_destroy(sb);
	} else {
		failures += check_int_m((allocs_to_fail == 0 ? 0 : 1), 1, msg);
	}

	failures += check_unsigned_int_m(ctx.frees, ctx.allocs, "alloc/free");
	failures +=
	    check_unsigned_int_m(ctx.free_bytes, ctx.alloc_bytes, "bytes");

	eembed_global_allocator = orig;

	return failures;
}

unsigned test_out_of_memory_construction_buf(const char *msg,
					     unsigned long allocs_to_fail)
{
	unsigned failures = 0;
	struct eembed_allocator *orig = eembed_global_allocator;
	struct eembed_allocator ea;
	struct echeck_err_injecting_context ctx;

	echeck_err_injecting_allocator_init(&ea, orig, &ctx, eembed_err_log);
	ctx.attempts_to_fail_bitmask = allocs_to_fail;

	eembed_global_allocator = &ea;

	unsigned char buf[65];

	const char *longstr = "123456789 123456789 123456789 1234567890";
	size_t longstr_len = eembed_strlen(longstr);
	strbuf_s *sb = strbuf_new_custom(&ea, buf, 65, longstr, longstr_len);
	if (sb) {
		failures += check_unsigned_long_m(allocs_to_fail, 0, msg);
		strbuf_destroy(sb);
	} else {
		failures += check_int_m((allocs_to_fail == 0 ? 0 : 1), 1, msg);
	}

	failures += check_unsigned_int_m(ctx.frees, ctx.allocs, "alloc/free");
	failures +=
	    check_unsigned_int_m(ctx.free_bytes, ctx.alloc_bytes, "bytes");

	eembed_global_allocator = orig;

	return failures;
}

/* TODO: Split into two tests? */
unsigned test_oom(void)
{
	unsigned failures = 0;

	failures += test_out_of_memory_construction("a", 0);
	failures += test_out_of_memory_construction("b", 1UL << 0);
	failures += test_out_of_memory_construction("c", 1UL << 1);

	failures += test_out_of_memory_construction_buf("d", 0);
	failures += test_out_of_memory_construction_buf("e", 1UL << 0);

	return failures;
}

ECHECK_TEST_MAIN(test_oom)
