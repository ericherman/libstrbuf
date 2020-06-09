default: check

SHELL=/bin/bash
BROWSER=firefox

#DEBUG_RUN=valgrind -q

# extracted from https://github.com/torvalds/linux/blob/master/scripts/Lindent
LINDENT=indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 -il0

CFLAGS += -g -Wall -Wextra -pedantic -Werror -pipe

BUILD_CFLAGS += -DNDEBUG -O2 -Wno-unused-parameter -Wno-unused-variable

DEBUG_CFLAGS += -DDEBUG -O0 \
	-fno-inline-small-functions \
	-fkeep-inline-functions \
	-fkeep-static-functions \
	-fprofile-arcs \
	-ftest-coverage \
	--coverage

DEBUG_LDFLAGS=-lgcov

TEST_BUILD_OBJS=strbuf.o echeck.o oom-injecting-malloc.o

TEST_DEBUG_OBJS=strbuf-debug.o echeck-debug.o oom-injecting-malloc-debug.o

TEST_INCS=-I./src \
	-I./tests \
	-I./submodules/libecheck/src \
	-I./submodules/context-alloc/src \
	-I./submodules/context-alloc/util

TEST_BUILD_CFLAGS=$(CFLAGS) $(BUILD_CFLAGS) $(TEST_INCS) $(TEST_BUILD_OBJS)
TEST_DEBUG_CFLAGS=$(CFLAGS) $(DEBUG_CFLAGS) $(TEST_INCS) $(TEST_DEBUG_OBJS)

# $@ : target label
# $< : the first prerequisite after the colon
# $^ : all of the prerequisite files
# $* : wildcard matched part

strbuf.o: src/strbuf.c src/strbuf.h
	$(CC) -c $(CFLAGS) $(BUILD_CFLAGS) -I./src $< -o $@

strbuf-debug.o: src/strbuf.c src/strbuf.h
	$(CC) -c $(CFLAGS) $(DEBUG_CFLAGS) -I./src $< -o $@


echeck.o:	submodules/libecheck/src/echeck.c \
		submodules/libecheck/src/echeck.h
	$(CC) -c  $(CFLAGS) $(BUILD_CFLAGS) \
		-I./submodules/libecheck/src \
		$< -o $@

echeck-debug.o: submodules/libecheck/src/echeck.c \
		submodules/libecheck/src/echeck.h
	$(CC) -c  $(CFLAGS) $(DEBUG_CFLAGS) \
		-I./submodules/libecheck/src \
		$< -o $@

oom-injecting-malloc.o:	\
		submodules/context-alloc/util/oom-injecting-malloc.c \
		submodules/context-alloc/util/oom-injecting-malloc.h \
		submodules/context-alloc/src/context-alloc.h
	$(CC) -c $(CFLAGS) $(BUILD_CFLAGS) \
		-I./submodules/context-alloc/src \
		-I./submodules/context-alloc/util \
		$< -o $@

oom-injecting-malloc-debug.o: \
		submodules/context-alloc/util/oom-injecting-malloc.c \
		submodules/context-alloc/util/oom-injecting-malloc.h \
		submodules/context-alloc/src/context-alloc.h
	$(CC) -c $(CFLAGS) $(DEBUG_CFLAGS) \
		-I./submodules/context-alloc/src \
		-I./submodules/context-alloc/util \
		$< -o $@

# append
test-append: tests/test-append.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-append-debug: tests/test-append.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-append: test-append
	./$<

check-append-debug: test-append-debug
	$(DEBUG_RUN) ./$<

# append_f
test-append-f: tests/test-append-f.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-append-f-debug: tests/test-append-f.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-append-f: test-append-f
	./$<

check-append-f-debug: test-append-f-debug
	$(DEBUG_RUN) ./$<

# append_float
test-append-float: tests/test-append-float.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-append-float-debug: tests/test-append-float.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-append-float: test-append-float
	./$<

check-append-float-debug: test-append-float-debug
	$(DEBUG_RUN) ./$<

# append_int
test-append-int: tests/test-append-int.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-append-int-debug: tests/test-append-int.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-append-int: test-append-int
	./$<

check-append-int-debug: test-append-int-debug
	$(DEBUG_RUN) ./$<

# append_uint
test-append-uint: tests/test-append-uint.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-append-uint-debug: tests/test-append-uint.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-append-uint: test-append-uint
	./$<

check-append-uint-debug: test-append-uint-debug
	$(DEBUG_RUN) ./$<

# prepend
test-prepend: tests/test-prepend.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-prepend-debug: tests/test-prepend.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-prepend: test-prepend
	$(DEBUG_RUN) ./$<

check-prepend-debug: test-prepend-debug
	$(DEBUG_RUN) ./$<

# prepend_f
test-prepend-f: tests/test-prepend-f.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-prepend-f-debug: tests/test-prepend-f.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-prepend-f: test-prepend-f
	./$<

check-prepend-f-debug: test-prepend-f-debug
	$(DEBUG_RUN) ./$<

# prepend_float
test-prepend-float: tests/test-prepend-float.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-prepend-float-debug: tests/test-prepend-float.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-prepend-float: test-prepend-float
	./$<

check-prepend-float-debug: test-prepend-float-debug
	$(DEBUG_RUN) ./$<

# prepend_int
test-prepend-int: tests/test-prepend-int.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-prepend-int-debug: tests/test-prepend-int.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-prepend-int: test-prepend-int
	./$<

check-prepend-int-debug: test-prepend-int-debug
	$(DEBUG_RUN) ./$<

# prepend_uint
test-prepend-uint: tests/test-prepend-uint.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-prepend-uint-debug: tests/test-prepend-uint.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-prepend-uint: test-prepend-uint
	./$<

check-prepend-uint-debug: test-prepend-uint-debug
	$(DEBUG_RUN) ./$<

# new-no-grow
test-new-no-grow: tests/test-new-no-grow.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-new-no-grow-debug: tests/test-new-no-grow.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-new-no-grow: test-new-no-grow
	./$<

check-new-no-grow-debug: test-new-no-grow-debug
	$(DEBUG_RUN) ./$<

# trim
test-trim: tests/test-trim.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-trim-debug: tests/test-trim.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-trim: test-trim
	./$<

check-trim-debug: test-trim-debug
	$(DEBUG_RUN) ./$<


# oom
test-oom: tests/test-oom.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-oom-debug: tests/test-oom.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-oom: test-oom
	./$<

check-oom-debug: test-oom-debug
	$(DEBUG_RUN) ./$<

# avail
test-avail: tests/test-avail.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

test-avail-debug: tests/test-avail.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-avail: test-avail
	./$<

check-avail-debug: test-avail-debug
	$(DEBUG_RUN) ./$<




check-build: \
	check-append \
	check-append-f \
	check-append-float \
	check-append-int \
	check-append-uint \
	check-prepend \
	check-prepend-f \
	check-prepend-float \
	check-prepend-int \
	check-prepend-uint \
	check-new-no-grow \
	check-trim \
	check-avail \
	check-oom

check-debug: \
	check-append-debug \
	check-append-f-debug \
	check-append-float-debug \
	check-append-int-debug \
	check-append-uint-debug \
	check-prepend-debug \
	check-prepend-f-debug \
	check-prepend-float-debug \
	check-prepend-int-debug \
	check-prepend-uint-debug \
	check-new-no-grow-debug \
	check-trim-debug \
	check-avail-debug \
	check-oom-debug

check: check-build check-debug

line-cov: check-debug
	lcov	--checksum \
		--capture \
		--base-directory . \
		--directory . \
		--output-file coverage.info

html-report: line-cov
	mkdir -pv ./coverage_html
	genhtml coverage.info --output-directory coverage_html

coverage: html-report
	$(BROWSER) ./coverage_html/src/strbuf.c.gcov.html

# .gitignore-based clean rules:
clean-git-ignore:
	rm -rvf `cat .gitignore | sed -e 's/#.*//'`

clean-git-ignore-src:
	pushd src && rm -rvf `cat ../.gitignore | sed -e 's/#.*//'` && popd

clean-git-ignore-tests:
	pushd tests && rm -rvf `cat ../.gitignore | sed -e 's/#.*//'` && popd

clean: clean-git-ignore \
	clean-git-ignore-src \
	clean-git-ignore-tests

mrproper:
	git clean -dffx
	git submodule foreach --recursive git clean -dffx

spotless: mrproper

tidy:
	$(LINDENT) \
		-T FILE -T size_t -T ssize_t -T bool \
		-T int8_t -T int16_t -T int32_t -T int64_t \
		-T uint8_t -T uint16_t -T uint32_t -T uint64_t \
		-T strbuf_s \
		tests/*.c \
		src/*.c src/*.h
