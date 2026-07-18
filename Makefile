TESTS:=
TESTS+=$(patsubst %.c,%,$(wildcard test/*.c))

.PHONY: default
default: tests

$(TESTS): ${TESTS:=.c}
	$(CC) $@.c src/buf.c -I test -I src -o $@

.PHONY: tests
tests: $(TESTS) ${TESTS:=.c}
	for test in $(TESTS); do \
		$$test ; \
	done

.PHONY: clean
clean:
	rm -f $(TESTS)

README.md: src/buf.h
	stddoc < src/buf.h > README.md
