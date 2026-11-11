#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "buf.h"

#define CHUNK_SIZE 4096

static char chunk[CHUNK_SIZE];

// Appending a block larger than double the current capacity must grow the
// buffer to fit it, not just double the old capacity. The growth path in
// buf_append used to compute MAX(32, cap * 2) without regard for the pending
// append length, overflowing the allocation on every such append.
void test_large_append_grows_to_fit() {
  struct buf subject;
  memset(&subject, 0, sizeof subject);

  ASSERT("Small append allocates initial capacity", buf_append(&subject, "hello", 5) == 0);
  ASSERT("Initial capacity is 32", subject.cap == 32);

  memset(chunk, 'A', sizeof chunk);
  ASSERT("Large append succeeds", buf_append(&subject, chunk, sizeof chunk) == 0);

  ASSERT("Capacity covers length after large append", subject.cap >= subject.len);
  ASSERT("Length includes large append", subject.len == 5 + CHUNK_SIZE);
  ASSERT("Original data survived large append", memcmp(subject.dat, "hello", 5) == 0);
  ASSERT("Large appended data is intact", memcmp(subject.dat + 5, chunk, CHUNK_SIZE) == 0);

  buf_clear(&subject);
}

// Same bug via the consume path: after buf_consume, the eject step reclaims
// head space but may still leave too little capacity, hitting the grow step.
void test_large_append_after_consume() {
  struct buf subject;
  memset(&subject, 0, sizeof subject);

  memset(chunk, 'B', sizeof chunk);
  ASSERT("Fill to initial capacity", buf_append(&subject, chunk, 5) == 0);
  ASSERT("Fill past initial capacity", buf_append(&subject, chunk, 40) == 0);
  ASSERT("Capacity doubled on overflow", subject.cap == 64);

  ASSERT("Consume head of buffer", buf_consume(&subject, 30) == 30);
  ASSERT("Data pointer moved past consumed bytes", subject.dat - subject.alloc == 30);

  ASSERT("Large append into consumed buffer succeeds", buf_append(&subject, chunk, sizeof chunk) == 0);

  ASSERT("Capacity covers length after large append", subject.cap >= subject.len);
  ASSERT("Length includes large append", subject.len == 15 + CHUNK_SIZE);
  ASSERT("Data pointer reset to allocation start", subject.dat == subject.alloc);
  ASSERT("Remaining data survived large append", memcmp(subject.dat, chunk, 15) == 0);
  ASSERT("Large appended data is intact", memcmp(subject.dat + 15, chunk, CHUNK_SIZE) == 0);

  buf_clear(&subject);
}

int main(int argc, const char *argv[]) {
  buf_set_allocator(realloc, free);

  RUN(test_large_append_grows_to_fit);
  RUN(test_large_append_after_consume);
  return TEST_REPORT();
}
