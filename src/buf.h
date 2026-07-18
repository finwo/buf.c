#ifndef __FINWO_BUF_H__
#define __FINWO_BUF_H__

#ifdef __cplusplus
extern "C" {
#endif

/// finwo/buf
/// =========
///
/// > Simple byte buffers
///
/// This library makes use of [dep](https://github.com/finwo/dep) to manage it's
/// dependencies and exports.
///
/// Installation
/// ------------
///
/// ```sh
/// dep add finwo/buf
/// dep install
/// ```
///
/// After that, simply add `include lib/.dep/config.mk` in your makefile and include
/// the header file by adding `#include "finwo/buf.h`.
///

#include <stddef.h>

/// API
/// ---

///
/// ### Structures
///

/// <details>
///   <summary>struct buf</summary>
///
///   The main handle of the buffer library
///<C
struct buf {
  char *alloc;
  char *dat;
  size_t len;
  size_t cap;
};
///>
/// </details>

///
/// ### Methods
///

/// <details>
///   <summary>buf_error(errno)</summary>
///
///   Translates an error code into something human-readable (or null)
///<C
const char *buf_error(int errno);
///>
/// </details>

/// <details>
///   <summary>buf_append(subject, source, length)</summary>
///
///   Append `length` bytes to the data in the buffer
///   Copies the data, does not mutate the source
///
///   Returns an error code, or 0 on success
///<C
int buf_append(struct buf *subject, const char *source, size_t length);
///>
/// </details>

/// <details>
///   <summary>buf_append_byte(subject, value)</summary>
///
///   Append a single byte to the buffer
///
///   Returns an error code, or 0 on success
///<C
int buf_append_byte(struct buf *subject, const char value);
///>
/// </details>


/// <details>
///   <summary>buf_consume(subject, length)</summary>
///
///   Remove `length` bytes from the front of the buffer
///
///   Returns the amount of bytes removed (may differ from directive)
///<C
size_t buf_consume(struct buf *subject, size_t length);
///>
/// </details>

/// <details>
///   <summary>buf_clear(subject)</summary>
///
///   Clears out all data from the buffer
///<C
int buf_clear(struct buf *subject);
///>
/// </details>

/**
 */
/// <details>
///   <summary>buf_set_allocator(realloc, free)</summary>
///
///    Sets custom allocator instead of libc. This function, if needed, should be
///    called only once at startup and prior to calling buf_* functions
///<C
void buf_set_allocator(void *(*realloc)(void*,size_t), void (*free)(void*));
///>
/// </details>

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __FINWO_BUF_H__

///
/// Testing
/// -------
///
/// If you want to run the library's tests, simply run `make tests` to compile
/// and run the testing binaries
///
/// License
/// -------
///
/// buf.c source code is available under the [FGPL License](License.md)
