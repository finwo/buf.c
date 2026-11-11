#include <string.h>
#include <stdlib.h>

#include "buf.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#define ARRAY_SIZE(x) ((sizeof x) / (sizeof *x))

const char *error_messages[] = {
  "Unknown error",
  "Missing subject",
  "Missing source",
};

#define BUF_ERRNO_UNKNOWN         -1
#define BUF_ERRNO_OK               0
#define BUF_ERRNO_MISSING_SUBJECT  1
#define BUF_ERRNO_MISSING_SOURCE   2

static void *(*_realloc)(void* ptr,size_t size) = realloc;
static void (*_free)(void* ptr) = free;

void buf_set_allocator(void *(*realloc)(void*,size_t), void (*free)(void*)) {
  _realloc = realloc;
  _free    = free;
}

const char *buf_error(int errno) {

  // No error
  if (!errno) return NULL;

  // Known error
  if ((errno > 0) && ((size_t)errno < ARRAY_SIZE(error_messages))) {
    return error_messages[errno];
  }

  // Unknown error
  return error_messages[0];
}

int buf_append(struct buf *subject, const char *source, const size_t length) {
  if (!subject) return BUF_ERRNO_MISSING_SUBJECT;
  if (!source) return BUF_ERRNO_MISSING_SOURCE;

  // Initial alloc if blank buffer
  if (!subject->alloc) {
    size_t l = MAX(32, length);
    subject->alloc = _realloc(NULL, l);
    subject->dat   = subject->alloc;
    subject->cap   = l;
    subject->len   = 0;
    goto lbl_buf_append_copy; // Simply skips cap checks
  }

  // Eject consumed data if we need space
  if (
      (subject->dat != subject->alloc) &&
      ((subject->len + length) > subject->cap)
  ) {
    size_t diff = subject->dat - subject->alloc;
    memmove(subject->alloc, subject->dat, subject->len);
    subject->cap += diff;
    subject->dat  = subject->alloc;
  }

  // Grow buffer if still not enough space: double, but never less than needed
  if ((subject->len + length) > subject->cap) {
    subject->cap   = MAX(subject->len + length, MAX(32, subject->cap * 2));
    subject->alloc = _realloc(subject->alloc, subject->cap);
    subject->dat   = subject->alloc;
  }

  // Add the data to the end
lbl_buf_append_copy:
  memcpy(subject->dat + subject->len, source, length);
  subject->len += length;

  return BUF_ERRNO_OK;
}

int buf_append_byte(struct buf *subject, const char value) {
  return buf_append(subject, &value, sizeof(char));
}

size_t buf_consume(struct buf *subject, size_t length) {
  if (!subject) return 0;
  if (length <= 0) return 0;

  size_t n = MIN(length, subject->len);
  subject->dat += n;
  subject->len -= n;
  subject->cap -= n;

  return n;
}

/**
 * Clears out all data from the buffer
 */
int buf_clear(struct buf *subject) {
  if (!subject) return BUF_ERRNO_MISSING_SUBJECT;

  if (subject->alloc) {
    _free(subject->alloc);
    subject->alloc = NULL;
  }

  subject->dat = NULL;
  subject->len = 0;
  subject->cap = 0;

  return BUF_ERRNO_OK;
}

#ifdef __cplusplus
} // extern "C" {
#endif // __cplusplus
