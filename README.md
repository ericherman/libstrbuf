`libstrbuf` is a mutable string library for C,
usable in embedded firmwares

Usage
-----
Include the header file:

```c
	#include <strbuf.h>
```

New instances are created with a call to one of the `strbuf_new` functions.

A simple string copy constructor:

```c
	const char *foo = "bar";
	size_t len = strlen(foo);
	strbuf_s *sb1 = strbuf_new(str, len);
```

A stack buffer constructor:

```c
	unsigned char buf[1024];
	const char *foo = "";
	size_t len = 0;
	strbuf_s *sbstack = strbuf_no_grow(buf, 1024, foo, len);
```

A full custom constructor with memory allocator:

```c
        const size_t bytes_len = 2048;
        unsigned char bytes[bytes_len];
        struct eembed_allocator *ea = eembed_bytes_allocator(bytes, bytes_len);

	size_t initial_buf_size = 0;
	const char *str = "foo";
	size_t len = strlen(str);

	strbuf_s *sb = strbuf_new_custom(ea, initial_mem_buf, initial_buf_size,
					 str, len);
```

The `strbuf_s` can be freed with:

```c
	strbuf_destroy(sb);
```

The current string can be obtained with a call to `strbuf_str`:

	printf("%s\n", strbuf_str(sb));

New values can be assigned with `strbuf_set`:

```c
	strbuf_set(sb, "foo", strlen("foo"));
```

Whitespace can be trimmed from the beginning, end, or both:

```c
	strbuf_trim_l(strbuf_s *sb); // trim leading whitespace
	strbuf_trim_r(strbuf_s *sb); // trim trailing whitespace
	strbuf_trim(strbuf_s *sb);   // trim both
```

An index-out-of-bounds safe `char_at` function:

```c
	char c = strbuf_char(sb, 4);
```

A variety of `append` and `prepend` functions:

```c
	const char *s;
	s = strbuf_append(sb, str, len);
	s = strbuf_prepend(sb, str, len);

	const char *format = "0x%x";
	size_t max = strlen("0xFFFFFFFF"); /* max additional size of string */
	s = strbuf_append_f(sb, max, format, ...);
	s = strbuf_prepend_f(sb, max, format, ...);

	long double f = 1.0/2.0;
	s = strbuf_append_float(sb, f);
	s = strbuf_prepend_float(sb, f);

	int64_t i = 23;
	s = strbuf_append_int(sb, i);
	s = strbuf_prepend_int(sb, i);

	uint64_t u = 42;
	s = strbuf_append_uint(sb, u);
	s = strbuf_prepend_uint(sb, u);
```

The raw buffer can be retrieved from the `strbuf`:

```c
	size_t buf_size = 0;
	char *buf = strbuf_expose(sb, &buf_size);
```

Once done with the raw buffer, it should be returned to the control of
the library, so the struct's end-pointer can be adjusted:

```c
	strbuf_return(sb);
```

Cloning
-------

```bash
git clone -o upstream https://github.com/ericherman/libstrbuf.git &&
 cd libstrbuf &&
 git submodule update --init --recursive &&
 make -j check
```

License
-------
GNU Lesser General Public License (LGPL), version 3 or later.

See [COPYING](COPYING) and [COPYING.LESSER](COPYING.LESSER) for details.
