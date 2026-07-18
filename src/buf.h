#ifndef __FINWO_BUF_H__
#define __FINWO_BUF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

struct buf {
  char *alloc;
  char *dat;
  size_t len;
  size_t cap;
};

/**
 * Append <length> bytes to the data in the buffer
 * Copies the data, does not mutate the source
 *
 * Returns an error code, or 0 on success
 */
int buf_append(struct buf *subject, const char *source, size_t length);

/**
 * Append single byte to the data in the buffer
 *
 * Returns an error code, or 0 on success
 */
int buf_append_byte(struct buf *subject, const char value);

/**
 * Remove <length> bytes from the front of the buffer
 */
size_t buf_consume(struct buf *subject, size_t length);

/**
 * Clears out all data from the buffer
 */
int buf_clear(struct buf *subject);

/**
 * Sets custom allocator instead of libc. This function, if needed, should be
 * called only once at startup and prior to calling buf_* functions
 */
void buf_set_allocator(void *(*realloc)(void*,size_t), void (*free)(void*));

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __FINWO_BUF_H__
