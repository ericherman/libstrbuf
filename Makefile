default: check

SHELL=/bin/bash
BROWSER=firefox

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

TEST_BUILD_OBJS=build/strbuf.o build/echeck.o build/oom-injecting-malloc.o
TEST_DEBUG_OBJS=debug/strbuf.o debug/echeck.o debug/oom-injecting-malloc.o

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

build:
	mkdir -pv build

debug:
	mkdir -pv debug


build/strbuf.o: src/strbuf.c src/strbuf.h build
	$(CC) -c $(CFLAGS) $(BUILD_CFLAGS) -I./src $< -o $@

debug/strbuf.o: src/strbuf.c src/strbuf.h debug
	$(CC) -c $(CFLAGS) $(DEBUG_CFLAGS) -I./src $< -o $@


build/echeck.o: submodules/libecheck/src/echeck.c \
		submodules/libecheck/src/echeck.h \
		build
	$(CC) -c  $(CFLAGS) $(BUILD_CFLAGS) \
		-I./submodules/libecheck/src \
		$< -o $@

debug/echeck.o: submodules/libecheck/src/echeck.c \
		submodules/libecheck/src/echeck.h \
		debug
	$(CC) -c  $(CFLAGS) $(DEBUG_CFLAGS) \
		-I./submodules/libecheck/src \
		$< -o $@

build/oom-injecting-malloc.o: \
		submodules/context-alloc/util/oom-injecting-malloc.c \
		submodules/context-alloc/util/oom-injecting-malloc.h \
		submodules/context-alloc/src/context-alloc.h \
		build
	$(CC) -c $(CFLAGS) $(BUILD_CFLAGS) \
		-I./submodules/context-alloc/src \
		-I./submodules/context-alloc/util \
		$< -o $@

debug/oom-injecting-malloc.o: \
		submodules/context-alloc/util/oom-injecting-malloc.c \
		submodules/context-alloc/util/oom-injecting-malloc.h \
		submodules/context-alloc/src/context-alloc.h \
		debug
	$(CC) -c $(CFLAGS) $(DEBUG_CFLAGS) \
		-I./submodules/context-alloc/src \
		-I./submodules/context-alloc/util \
		$< -o $@

# append
build/test-append: tests/test-append.c $(TEST_BUILD_OBJS) build
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-append: tests/test-append.c $(TEST_DEBUG_OBJS) debug
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-append: build/test-append
	./$<

check-append-debug: debug/test-append
	$(DEBUG_RUN) ./$<

# append_f
build/test-append-f: tests/test-append-f.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-append-f: tests/test-append-f.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-append-f: build/test-append-f
	./$<

check-append-f-debug: debug/test-append-f
	$(DEBUG_RUN) ./$<

# append_float
build/test-append-float: tests/test-append-float.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-append-float: tests/test-append-float.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-append-float: build/test-append-float
	./$<

check-append-float-debug: debug/test-append-float
	$(DEBUG_RUN) ./$<

# append_int
build/test-append-int: tests/test-append-int.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-append-int: tests/test-append-int.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-append-int: build/test-append-int
	./$<

check-append-int-debug: debug/test-append-int
	$(DEBUG_RUN) ./$<

# append_uint
build/test-append-uint: tests/test-append-uint.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-append-uint: tests/test-append-uint.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-append-uint: build/test-append-uint
	./$<

check-append-uint-debug: debug/test-append-uint
	$(DEBUG_RUN) ./$<

# prepend
build/test-prepend: tests/test-prepend.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-prepend: tests/test-prepend.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-prepend: build/test-prepend
	./$<

check-prepend-debug: debug/test-prepend
	$(DEBUG_RUN) ./$<

# prepend_f
build/test-prepend-f: tests/test-prepend-f.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-prepend-f: tests/test-prepend-f.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-prepend-f: build/test-prepend-f
	./$<

check-prepend-f-debug: debug/test-prepend-f
	$(DEBUG_RUN) ./$<

# prepend_float
build/test-prepend-float: tests/test-prepend-float.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-prepend-float: tests/test-prepend-float.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-prepend-float: build/test-prepend-float
	./$<

check-prepend-float-debug: debug/test-prepend-float
	$(DEBUG_RUN) ./$<

# prepend_int
build/test-prepend-int: tests/test-prepend-int.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-prepend-int: tests/test-prepend-int.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-prepend-int: build/test-prepend-int
	./$<

check-prepend-int-debug: debug/test-prepend-int
	$(DEBUG_RUN) ./$<

# prepend_uint
build/test-prepend-uint: tests/test-prepend-uint.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-prepend-uint: tests/test-prepend-uint.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-prepend-uint: build/test-prepend-uint
	./$<

check-prepend-uint-debug: debug/test-prepend-uint
	$(DEBUG_RUN) ./$<

# new-no-grow
build/test-new-no-grow: tests/test-new-no-grow.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-new-no-grow: tests/test-new-no-grow.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-new-no-grow: build/test-new-no-grow
	./$<

check-new-no-grow-debug: debug/test-new-no-grow
	$(DEBUG_RUN) ./$<

# trim
build/test-trim: tests/test-trim.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-trim: tests/test-trim.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-trim: build/test-trim
	./$<

check-trim-debug: debug/test-trim
	$(DEBUG_RUN) ./$<


# oom
build/test-oom: tests/test-oom.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-oom: tests/test-oom.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-oom: build/test-oom
	./$<

check-oom-debug: debug/test-oom
	$(DEBUG_RUN) ./$<

# avail
build/test-avail: tests/test-avail.c $(TEST_BUILD_OBJS)
	$(CC) $(TEST_BUILD_CFLAGS) $< -o $@

debug/test-avail: tests/test-avail.c $(TEST_DEBUG_OBJS)
	$(CC) $(TEST_DEBUG_CFLAGS) $< -o $@ $(DEBUG_LDFLAGS)

check-avail: build/test-avail
	./$<

check-avail-debug: debug/test-avail
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
