#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "buf.h"

// Append paths after buf_consume moved dat past alloc

void test_append_reuses_tail_capacity() {
  struct buf subject;
  memset(&subject, 0, sizeof subject);

  ASSERT("Fill buffer", buf_append(&subject, "0123456789", 10) == 0);
  ASSERT("Consume head of buffer", buf_consume(&subject, 4) == 4);

  // 6 + 20 <= 28 available, so neither eject nor grow may run
  ASSERT("Append into remaining tail capacity", buf_append(&subject, "ABCDEFGHIJKLMNOPQRST", 20) == 0);

  ASSERT("Data pointer unchanged by in-place append", subject.dat - subject.alloc == 4);
  ASSERT("Capacity unchanged by in-place append", subject.cap == 28);
  ASSERT("Length includes append", subject.len == 26);
  ASSERT("Appended data follows unconsumed data", memcmp(subject.dat, "456789ABCDEFGHIJKLMNOPQRST", 26) == 0);

  buf_clear(&subject);
}

void test_append_ejects_without_growing() {
  struct buf subject;
  memset(&subject, 0, sizeof subject);

  ASSERT("Fill buffer", buf_append(&subject, "0123456789", 10) == 0);
  ASSERT("Consume head of buffer", buf_consume(&subject, 8) == 8);

  // 2 + 25 > 24 available, but the 8 consumed bytes reclaim enough:
  // eject must run and grow must not
  ASSERT("Append forcing eject", buf_append(&subject, "abcdefghijklmnopqrstuvwxy", 25) == 0);

  ASSERT("Data pointer reset to allocation start", subject.dat == subject.alloc);
  ASSERT("Eject reclaimed consumed head space", subject.cap == 32);
  ASSERT("Length includes append", subject.len == 27);
  ASSERT("Appended data follows unconsumed data", memcmp(subject.dat, "89abcdefghijklmnopqrstuvwxy", 27) == 0);

  buf_clear(&subject);
}

int main(int argc, const char *argv[]) {
  buf_set_allocator(realloc, free);

  RUN(test_append_reuses_tail_capacity);
  RUN(test_append_ejects_without_growing);
  return TEST_REPORT();
}
