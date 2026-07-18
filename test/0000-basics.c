#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "buf.h"

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

void test_basic() {
  struct buf *buf = malloc(sizeof(struct buf));
  buf_clear(buf);

  ASSERT("Allocation of buffer", buf != NULL);
  ASSERT("Cleared buf has null alloc", buf->alloc == NULL);
  ASSERT("Cleared buf has null data", buf->dat == NULL);
  ASSERT("Cleared buf has 0 length", buf->len == 0);
  ASSERT("Cleared buf has 0 capacity", buf->cap == 0);

  buf_append_byte(buf, 'H');

  ASSERT("Appended buffer has alloc", buf->alloc != NULL);
  ASSERT("Appended buffer has data", buf->dat != NULL);
  ASSERT("Appended buffer has matching alloc and data", buf->dat == buf->alloc);
  ASSERT("Appended buffer has correct length", buf->len == 1);
  ASSERT("Appended buffer has correct capacity", buf->cap == 32);

  buf_append(buf, "ello World", 10);

  ASSERT("Appended buffer has matching alloc and data", buf->dat == buf->alloc);
  ASSERT("Appended buffer has correct length", buf->len == 11);
  ASSERT("Appended buffer has correct capacity", buf->cap == 32);
  ASSERT("Appended buffer has correct data", strncmp(buf->dat, "Hello World", MIN(buf->len, 11)) == 0);

  buf_consume(buf, 6);

  ASSERT("Consumed buffer has matching alloc and data", buf->dat - buf->alloc == 6);
  ASSERT("Consumed buffer has correct length", buf->len == 5);
  ASSERT("Consumed buffer has correct capacity", buf->cap == 26);
  ASSERT("Consumed buffer has correct data", strncmp(buf->dat, "World", MIN(buf->len, 5)) == 0);
}

int main(int argc, const char *argv[]) {
  buf_set_allocator(realloc, free);

  RUN(test_basic);
  return TEST_REPORT();
}
