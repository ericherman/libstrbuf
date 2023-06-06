`libstrbuf` is a mutable string library for C.
By design, it is usable in embedded firmwares as well as regular C programs.

Dependencies
------------
Sources for dependencies are included via git's submodule facility.
`libstrbuf` depends upon E(asy)Embed which is part of `libecheck`.
The tests depend upon E(asy)Check which is also a part of `libecheck`.
See: [`libecheck`](https://github.com/ericherman/libecheck).

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

To interoperate with code that expects a NULL-terminated char buffer,
the underlying raw buffer can be retrieved from the `strbuf_s`.
The `strbuf_expose` function ensures that the string contents start at `buf[0]`,
even if the internal start pointer had moved previously.

```c
	size_t buf_size = 0;
	char *buf = strbuf_expose(sb, &buf_size);
```

Once done with the underlying raw buffer, it should be returned to the control
of the library, so the struct's end-pointer can be adjusted to match the new
location of the first NULL byte:

```c
	strbuf_return(sb);
```

If confident that an existing buffer is at least `strbuf_struct_size()` larger
than the current contents, a `strbuf_s` can be constructed without allocation:

```c
/* demo-no-allocation-strbuf.c */
/* gcc -I src \
	src/strbuf.c \
	-I submodules/libecheck/src \
	submodules/libecheck/src/eembed.c \
	demo-no-allocation-strbuf.c \
	-o demo-no-allocation-strbuf &&
   ./demo-no-allocation-strbuf
*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <strbuf.h>

int main(void)
{
	char buf[4000];
	size_t buf_size = sizeof(buf);
	strcpy(buf, "foo");

	size_t len = strlen(buf);
	size_t avail = buf_size - len;
	size_t avail_needed = strbuf_struct_size();
	assert(avail_needed <= 64);
	assert(avail_needed <= avail);

	strbuf_s *sb = strbuf_no_grow((unsigned char *)buf, buf_size, buf, len);
	assert(sb);

	strbuf_prepend(sb, " my ", 4);
	strbuf_append(sb, " stuff ", 7);
	strbuf_trim(sb);

	strbuf_expose(sb, NULL);

	assert(strcmp(buf, "my foo stuff") == 0);
	printf("%s\n", buf);

	return 0;
}
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
