#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct {
	void*  ptr;
	size_t sz;
} buf_s;

typedef struct {
    int   err;
    buf_s buf;
} rbuf_s;


rbuf_s rd_file (const char* pathname);

int    wr_file (const char* pathname, buf_s buf);
int    wr_file2(const char* pathname, buf_s head, buf_s buf);

#ifdef __cplusplus
}
#endif

